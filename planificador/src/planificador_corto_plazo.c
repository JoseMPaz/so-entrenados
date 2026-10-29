#include "planificador_corto_plazo.h"

void * planificar_por_fifo (void * arg)
{
    

    pthread_exit (NULL);
}
/*pthread_t hilo_ingresar_job_a_exec;
    pthread_t hilo_atender_cores;
    pthread_t hilo_despertar_job;*/

    // 1. Motor READY -> EXEC (Busca procesos en Ready y Cores libres)
    //pthread_create (&hilo_ingresar_job_a_exec, NULL, ingresar_jobs_to_exec, NULL);
    
    // 2. Motor de Monitoreo/Egreso (Escucha de forma general o gestiona la delegación de ráfagas)
    //pthread_create (&hilo_atender_cores, NULL, gestionar_egresos_de_cores, NULL);
    
    // 3. Motor BLOCK -> READY (Despierta a los procesos bloqueados)
    //pthread_create (&hilo_despertar_job, NULL, despertar_job, NULL);
	
    // Sincronización estructural: Esperamos a que los 3 motores terminen si se apaga el sistema
    //pthread_join(hilo_ingresar_job_a_exec, NULL);
    //pthread_join(hilo_atender_cores, NULL);
    //pthread_join(hilo_despertar_job, NULL);


/*void * planificar_por_fifo (void * arg)
{
	// Bucle persistente protegido por control atómico para permitir un apagado limpio con Ctrl+C
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		// Sincronización: Espera estructural a que exista al menos un proceso en la cola de READY
		//Lo incrementa al ingregar un job de new a ready o bien al ingresar de block a ready, en
		//caso de rr, lo ingrementa por interrupción de fin de quantum
		sem_wait (&sem_pasaje_de_job_a_ready);*/
		
		/*Sincronización: Espera permanente a un core disponible, cuando:
		el servidor_planificador ingresa un nuevo core al sistema
		o una Syscall liberen un Core (hacen sem_post)*/
		/*sem_wait (&sem_core_disponible);*/
		
		/* --- SECCIÓN CRÍTICA 1: Búsqueda y Asignación de CPU (Core) --- */
		// Exclusión Mutua: Protegemos la lista de Cores ya que el servidor multihilo puede agregar Cores en paralelo
		/*sem_wait(&mutex_cores);
		t_core * core_elegido = buscar_primer_core_libre (cores);
		if (core_elegido != NULL) //Encontro al core libre
			core_elegido->estado = CORE_OCUPADO;//Cambia su estado
		sem_post(&mutex_cores);
		
		// Control de consistencia en caso de un despertar espurio o interrupción por salida
		if (core_elegido == NULL) //Se supone que no suceda
		{
			// Si por algún motivo de sincronización extrema no hay Core, devolvemos las fichas y reintentamos
			sem_post(&sem_pasaje_de_job_a_ready);
			sem_post(&sem_core_disponible);
			continue;
		}*/

		/* --- SECCIÓN CRÍTICA 2: Extracción del Proceso (Job) --- */
		// Exclusión Mutua: Evitamos que hilos de I/O o Admisión alteren la cola de READY en este milisegundo
		/*sem_wait(&mutex_ready);
		t_jcb * job_a_ejecutar = list_remove(cola_de_ready, PRIMER_ELEMENTO);//Extracción estricta por orden de llegada (FIFO)
		sem_post(&mutex_ready);
		
		// --- TRANSICIÓN DE ESTADO Y MÉTRICAS [pág. 10, 12] ---
		int64_t timestamp_actual = temporal_gettime (reloj_sistema);
		
		// Acumulamos el tiempo real que pasó esperando de forma exclusiva en la cola de READY [pág. 12]
		int64_t tiempo_en_ready = timestamp_actual - job_a_ejecutar->tiempo_ultima_transicion;
		job_a_ejecutar->estadisticas.tiempo_acumulado_espera_ready += tiempo_en_ready;
		
		// Incrementamos la métrica obligatoria de transiciones READY => EXEC [pág. 12]
		job_a_ejecutar->estadisticas.cantidad_transiciones_ready_exec++;
		
		// Actualizamos los campos administrativos del JCB antes de mandarlo a la CPU [pág. 10]
		job_a_ejecutar->tiempo_ultima_transicion = timestamp_actual; // Marca el inicio de su ráfaga EXEC
		job_a_ejecutar->estado = ESTADO_EXEC;
		
		// LOG MÍNIMO Y OBLIGATORIO: Registro de cambio de estado exigido por el enunciado [pág. 13]
		log_info(historial, "## (%u) Pasa del estado READY al estado EXEC", job_a_ejecutar->jid);
		*/
		/* --- CREACIÓN DEL HILO ASINCRÓNICO DE EJECUCIÓN --- */
		// Reservamos memoria para pasarle los parámetros al hilo sin Race Conditions
		/*t_args_despacho * args_hilo = (t_args_despacho *) malloc(sizeof(t_args_despacho));
		args_hilo->core = core_elegido;
		args_hilo->job = job_a_ejecutar;

		pthread_t thread_despacho;
		
		if (pthread_create(&thread_despacho, NULL, ejecutar_job_en_core, args_hilo) == 0) 
		{
			// Detachamos para que el hilo libere sus recursos de forma autónoma al terminar
			pthread_detach(thread_despacho); 
		} 
		else
		{
			log_error(historial, "Error crítico al crear el hilo de despacho para el JID %u", job_a_ejecutar->jid);
			// Rollback de emergencia en caso de fallo de recursos del S.O.
			free(args_hilo);
			// (Deberías manejar acá re-insertar el job a Ready o alertar un abort)
			
		}
	}
	
	pthread_exit(NULL);
}*/
/*
bool _es_libre(void * elemento) 
{
	t_core * core = (t_core *) elemento;
	return core->estado == CORE_LIBRE;
}
*/
/**
 * @brief Función auxiliar para buscar el primer procesador ocioso en el cluster.
 * @note Debe ejecutarse dentro de la exclusión mutua de mutex_cores.
 */
/*t_core * buscar_primer_core_libre(t_list * lista_cores)
{
	// Retorna el primer nodo que cumpla con la condición o NULL si están todos saturados
	return (t_core *) list_find (lista_cores, _es_libre);
}*/

/**
 * @brief Serializa y despacha el contexto de ejecución a la CPU elegida.
 * @details Envía el JID junto con la copia actual de todos los registros del procesador [pág. 10].
 */ /*
void despachar_contexto_a_core (int socket_core, t_jcb *job)
{
    // 1. Creamos la estructura con los datos exactos que pide el Core [pág. 10]
    t_payload_despacho payload;
    payload.jid = job->jid;
    payload.contexto = job->contexto_de_ejecucion;

    // 2. Usamos tu función nativa para inicializar el paquete con el op_code correcto
    t_paquete *paquete = crear_paquete(PLANIF_TO_CORE_EJECUTAR_JOB);

    // 3. Serializamos los bytes de la estructura adentro del paquete.
    // NOTA: Si tu estructura 't_paquete' en utils tiene un campo directo tipo 'void* stream' y 'uint32_t size',
    // asignalos acá. Si usás una función como 'agregar_a_paquete', llamala así:
    // agregar_a_paquete(paquete, &payload, sizeof(t_payload_despacho));
    
    // Si tu paquete maneja el stream de forma directa expuesto en la estructura, hacé esto:
    paquete->carga_util = malloc(sizeof(t_payload_despacho)); 
    memcpy(paquete->carga_util, &payload, sizeof(t_payload_despacho));
    // paquete->tamanio = sizeof(t_payload_despacho); // <-- Ajustalo al nombre real de tu campo si tiene tamaño.

    // 4. Enviamos el paquete serializado al Core a través de la red
    enviar_paquete(paquete, socket_core);

    // 5. Liberamos la memoria usando tu destructor nativo para evitar leaks
    destruir_paquete(paquete);
}
*/
/*
void* ejecutar_job_en_core(void* void_args) 
{
    t_args_despacho* args = (t_args_despacho*) void_args;
    t_core* core_elegido = args->core;     
    t_jcb* job_ejecutando = args->job;

    // 1. Enviamos el contexto
    despachar_contexto_a_core(core_elegido->socket, job_ejecutando);

    // 2. Esperamos la respuesta del Core
    t_operacion operacion = recibir_operacion(core_elegido->socket); 
    
    // 3. Procesamos el desalojo (Acá usás la variable 'operacion' para evitar el warning)
    switch(operacion) 
    {
        // case DESALOJO_QUANTUM: ...
        // case SYSCALL_EXIT: ...
		case CORE_TO_PLANIF_SYSCALL_EXIT:
            // 1. Recibir el paquete con el contexto de ejecución actualizado [pág. 10]
            // Asumiendo que tenés una función nativa en tus utils para recibir la carga útil
            //t_paquete* paquete_recibido = recibir_paquete(core_elegido->socket);
            
            // Actualizás el contexto del JCB con lo que te devuelve la CPU (registros modificados, etc.)
            // Ajustá este casteo e igualación según cómo manejes la estructura de tu stream/payload
            //t_payload_despacho* payload_rta = (t_payload_despacho*) paquete_recibido->carga_util;
           // job_ejecutando->contexto_de_ejecucion = payload_rta->contexto;
            
            // Liberamos la memoria del paquete que entró por red
            destruir_paquete(paquete_recibido);

            // 2. --- TRANSICIÓN DE ESTADO Y MÉTRICAS A EXIT [pág. 11, 12] ---
            int64_t timestamp_actual = temporal_gettime(reloj_sistema);
            
            // Calculamos cuánto duró su última ráfaga en EXEC y actualizamos métricas
            int64_t duracion_rafaga = timestamp_actual - job_ejecutando->tiempo_ultima_transicion;
            job_ejecutando->estadisticas.tiempo_total_ejecucion += duracion_rafaga;

            // Cambiamos el estado administrativo del JCB
            job_ejecutando->tiempo_ultima_transicion = timestamp_actual;
            job_ejecutando->estado = ESTADO_EXIT;

            // LOG MÍNIMO Y OBLIGATORIO: Registro exigido por el enunciado [pág. 13]
            log_info(historial, "## (%u) Pasa del estado EXEC al estado EXIT", job_ejecutando->jid);

            // 3. --- SECCIÓN CRÍTICA: Inserción en la cola de EXIT ---
            // Exclusión Mutua: Protegemos la cola de EXIT ya que el planificador de largo plazo puede estar leyendo de ella
            sem_wait(&mutex_exit);
            list_add(cola_de_exit, job_ejecutando);
            sem_post(&mutex_exit);

            // Sincronización: Avisamos al Planificador de Largo Plazo que hay un Job listo para ser destruido/liberado
            sem_post(&sem_pasaje_de_job_a_exit); 

            break;
                
        default:
            log_warning(historial, "Se recibió una operación no contemplada: %d", operacion);
            break;
    }

    // 4. Liberamos el Core
    sem_wait(&mutex_cores);
    core_elegido->estado = CORE_LIBRE;
    sem_post(&mutex_cores);

    // Avisamos que hay un nuevo core disponible para el planificador
    sem_post(&sem_core_disponible);

    free(args);
    return NULL;
}*/





void * planificar_por_rr (void * arg)
{

	pthread_exit(NULL);
}

void * planificar_por_hrrn (void * arg)
{

	pthread_exit(NULL);
}

void establecer_planificador_corto_plazo (char * algoritmo)
{
	if (strcmp (algoritmo, "FIFO") == 0)
		planificar_corto_plazo = planificar_por_fifo;
		
	if (strcmp (algoritmo, "RR") == 0)
		planificar_corto_plazo = planificar_por_rr;
	
	if (strcmp (algoritmo, "HRRN") == 0)
		planificar_corto_plazo = planificar_por_hrrn;

	return;
}
