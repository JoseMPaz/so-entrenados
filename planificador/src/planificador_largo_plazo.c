#include "planificador_largo_plazo.h"

void * planificar_largo_plazo (void * arg)
{
	// 💡 SOLUCIÓN HELGRIND: Declaramos los hilos de forma local. 
	// Al no usar variables globales, el manejador de señales no genera Data Races.
	pthread_t hilo_ingreso_de_jobs_local;
	pthread_t hilo_egreso_de_jobs_local;

	// Lanza los hilos independientes para paralelizar la entrada y la salida de procesos del cluster
	pthread_create (&hilo_ingreso_de_jobs_local, NULL, mover_jobs_de_new_a_ready, NULL);
	pthread_create (&hilo_egreso_de_jobs_local, NULL, eliminar_jobs_de_exit, NULL);
	
	// El hilo se queda esperando pacientemente que sus hilos hijos terminen solos ante un Ctrl+C
	pthread_join (hilo_ingreso_de_jobs_local, NULL);
	pthread_join (hilo_egreso_de_jobs_local, NULL);
    
	pthread_exit(NULL);
}

void * mover_jobs_de_new_a_ready (void * arg)
{
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		sem_wait (&sem_ingresar_job_a_new);
		// ⚠️ CONTROL DE SALIDA: Si nos despertaron artificialmente para salir, rompemos el bucle inmediatamente
		if (__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) break;

		sem_wait (&sem_grado_multiprogramacion);
		if (__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) {
			sem_post(&sem_grado_multiprogramacion); // Devolvemos el token si salimos antes de tiempo
			break;
		}
		
		// Se extrae el job de new de forma segura
		sem_wait (&mutex_new);
		t_jcb * primer_job = list_remove (cola_de_new, PRIMER_ELEMENTO); 
		sem_post (&mutex_new);
		
		// 🔥 CORRECCIÓN CRÍTICA CONTRA DEADLOCKS ANTE CTRL+C:
		if (primer_job == NULL) {
			sem_post(&sem_grado_multiprogramacion); // Devolvemos el token que consumimos
			
			// Si la lista dio NULL porque apretamos Ctrl+C, salimos inmediatamente del bucle
			if (__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) {
				break;
			}
			continue;
		}
		
		// --- CÁLCULO DE MÉTRICAS ESTIMADAS Y TRANSICIONES ---
		int64_t tiempo_actual = temporal_gettime (reloj_sistema);
		
		int64_t tiempo_en_new = tiempo_actual - primer_job->tiempo_ultima_transicion;
		primer_job->estadisticas.tiempo_acumulado_ingresar_new += tiempo_en_new;
		
		primer_job->tiempo_ingreso_a_ready = tiempo_actual;  
		primer_job->tiempo_ultima_transicion = tiempo_actual; 
		primer_job->estado = ESTADO_READY;
		
		// Se ingresa el job a ready
		sem_wait (&mutex_ready);
		list_add (cola_de_ready, primer_job);
		sem_post (&mutex_ready);
		
		log_info(historial, "## (%u) Pasa del estado NEW al estado READY", primer_job->jid);
		
		sem_post(&sem_ingresar_job_a_ready);
	}
	
	pthread_exit(NULL);
}

void * eliminar_jobs_de_exit (void * arg)
{
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		sem_wait (&sem_ingresar_job_a_exit);
		// ⚠️ CONTROL DE SALIDA: Si nos despertaron artificialmente para salir, rompemos el bucle inmediatamente
		if (__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) break;
		
		// Se extrae el job de exit
		sem_wait (&mutex_exit);
		t_jcb * primer_job = list_remove (cola_de_exit, PRIMER_ELEMENTO); 
		sem_post (&mutex_exit);
		
		if (primer_job == NULL) continue;
		
		/* TODO: Libera la imagen del job en Placa enviando la solicitud correspondiente por red */
		// enviar_liberacion_estructuras_a_placa(primer_job->jid);
		
		/* TODO: Imprime las estadísticas finales consolidadas del Job */
		// log_info(historial, "## (%u) finalizó su ejecución...", primer_job->jid);
		
		free (primer_job);
		
		sem_post (&sem_grado_multiprogramacion);
	}
	
	pthread_exit(NULL);
}

