#include "main.h"

/*
Compilar modulo desde archivo makefile: 
	make
Ejecutar Normal:
	./bin/planificador planificador.config no_hacer_nada.asm
Ejecutar con valgrind para controlar fuga de memoria:
	valgrind --leak-check=full --show-leak-kinds=all ./bin/planificador planificador.config no_hacer_nada.asm
Ejecutar con valgrind para controlar condiciones de carrera:
	valgrind --tool=helgrind ./bin/planificador planificador.config no_hacer_nada.asm
*/

int main(int argc, char * argv[]) 
{
	 configurar_senales_planificador();
	
	t_validar_argumentos validar_argumentos = validar_argumentos_planificador; // Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_planificador; // Setea la funcion cargar_configuracion
	t_paquete * paquete;
	t_operacion operacion;
	pthread_t hilo_planificador_largo_plazo;
	// pthread_t hilo_planificador_corto_plazo;
	
	// Establece el conteo en 0[ms]
	reloj_sistema = temporal_create();
	
	validar_argumentos (argc, argv);
	t_config * configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	
	historial = crear_historial ("planificador.log", "PLANIFICADOR_LOG", log_level); //historial es global
	
	cores = list_create (); // Esta lista permite administrar los cores que se tiene a disposicion para ejecutar los Jobs
	cola_de_new = list_create ();
	cola_de_ready = list_create ();
	cola_de_exec = list_create ();
	cola_de_block = list_create ();
	cola_de_exit = list_create ();
	
	establecer_planificador_corto_plazo (algoritmo_planificacion);
	
	inicializar_semaforos ();
	
	/* == Inicio Handshake Planificador - Placa == */
	socket_planificador_to_placa = crear_socket (CLIENTE, ip_placa, puerto_placa);
	conectar_a_servidor (socket_planificador_to_placa, ip_placa, puerto_placa);
	log_info (historial, "%s%s", "Solicitud de conexión: ", "## Módulo: Placa");
	paquete = crear_paquete (SOLICITUD_CONEXION_PLANIF_TO_PLACA);
	enviar_paquete (paquete, socket_planificador_to_placa);
	destruir_paquete (paquete);
	operacion = recibir_operacion (socket_planificador_to_placa);

	if (operacion == RESPUESTA_CONEXION_PLACA_TO_PLANIF_OK)
	{
		log_info (historial, "%s%s", "Conexión confirmada: ", "## Módulo: Placa");
	}
	else
	{
		fprintf (stderr, "%s\n", "Error: No se pudo conectar Planificador a Placa");
		exit(1);
	}
	/* == Final Handshake Planificador - Placa == */

	
	/*Servidor para aceptar conexiones de core*/
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);
	
	// == Inicio - Creacion del Job Padre ==
	t_jcb * job_padre = (t_jcb *) malloc (sizeof(t_jcb));
	
	/*Inicio - Carga del JOB*/
	job_padre->jid = generar_jid ();
	
	job_padre->estado = ESTADO_NEW;
	
	job_padre->contexto_de_ejecucion.pc = 0;  
	job_padre->contexto_de_ejecucion.p0 = 0;
	job_padre->contexto_de_ejecucion.p1 = 0;
	job_padre->contexto_de_ejecucion.p2 = 0;
	job_padre->contexto_de_ejecucion.p3 = 0;
	job_padre->contexto_de_ejecucion.p4 = 0;
	job_padre->contexto_de_ejecucion.p5 = 0;
	job_padre->contexto_de_ejecucion.ax = 0;
	job_padre->contexto_de_ejecucion.bx = 0;
	job_padre->contexto_de_ejecucion.cx = 0;
	job_padre->contexto_de_ejecucion.dx = 0;
	job_padre->contexto_de_ejecucion.e1 = 0;
	job_padre->contexto_de_ejecucion.e2 = 0;
	job_padre->contexto_de_ejecucion.e3 = 0;
	
	job_padre->estadisticas.tiempo_total_ejecucion = 0;
	job_padre->estadisticas.tiempo_acumulado_espera_ready = 0;
	job_padre->estadisticas.tiempo_acumulado_ingresar_new = 0;
	job_padre->estadisticas.max_memoria_dinamica_ocupada = 0;
	job_padre->estadisticas.cantidad_syscalls = 0;
	job_padre->estadisticas.cantidad_page_faults = 0;
	job_padre->estadisticas.cantidad_transiciones_ready_exec = 0;
	
	int64_t tiempo_actual = temporal_gettime (reloj_sistema);
	job_padre->tiempo_ultima_transicion = tiempo_actual;
	
	if (strcmp(algoritmo_planificacion, "HRRN") == 0) 
		job_padre->proxima_estimacion_rafaga = estimacion_inicial; 
	else 
		job_padre->proxima_estimacion_rafaga = 0;
		
	job_padre->tiempo_ingreso_a_ready = 0;
	/*Fin - Carga del JOB*/
	
	log_info(historial, "## (%u) Se crea el Job - Estado: NEW", job_padre->jid);

	// Se agrega el job a la cola de NEW de manera sincronizada
	sem_wait(&mutex_new);
		list_add(cola_de_new, job_padre);
	sem_post(&mutex_new);
	
	// Avisamos al planificador de largo plazo que hay un nuevo proceso en NEW listo para pasar a READY
    sem_post(&sem_ingresar_job_a_new); 

	
	// == Fin - Creacion del Job Padre ==

	// 🔥 CORRECCIÓN: Pasamos por referencia la variable global para asegurar que close() impacte de inmediato
	pthread_create (&hilo_servidor, NULL, servidor_planificador, (void *) &socket_escucha);
	

	pthread_create (&hilo_planificador_largo_plazo, NULL, planificar_largo_plazo, NULL);
	

	// pthread_create (&hilo_planificador_corto_plazo, NULL, planificar_corto_plazo, NULL);

	

	pthread_join (hilo_servidor, NULL);

	pthread_join (hilo_planificador_largo_plazo, NULL);

	// pthread_join (hilo_planificador_corto_plazo, NULL);

	
	destruir_historial (historial);
	destruir_configuracion (configuracion);
	close(socket_planificador_to_placa);
	sem_destroy(&mutex_cores);
	destruir_lista_cores (cores);
	
	if (cola_de_new  != NULL)
		list_destroy(cola_de_new);
	if (cola_de_ready  != NULL)
		list_destroy(cola_de_ready);
	if (cola_de_exec != NULL)
		list_destroy(cola_de_exec);
	if (cola_de_block != NULL)
		list_destroy(cola_de_block);
	if (cola_de_exit  != NULL)
		list_destroy(cola_de_exit);
		
	temporal_destroy(reloj_sistema);
	saludar("planificador");
	return 0;
}

void inicializar_semaforos (void)
{
	if (sem_init(&mutex_cores, 0, 1 /* Protege un recurso lista */) != 0) 
	{
		perror("Error al inicializar el semáforo mutex_cores");
		exit (EXIT_FAILURE);
	}
	
	if (sem_init(&sem_core_disponible, 0, 0 /* Inicia sin cores disponibles */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	
	if (sem_init(&sem_grado_multiprogramacion, 0, grado_multiprogramacion) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	
	if (sem_init(&sem_ingresar_job_a_new, 0, 0 /* Indica el ingreso a la cola de new */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	if (sem_init(&sem_ingresar_job_a_ready, 0, 0 /* Pausa para pasar jobs de cola_de_ready */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	if (sem_init(&sem_ingresar_job_a_exit, 0, 0 /* Pausa para destruir jobs de cola_de_exit */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	if (sem_init(&mutex_new, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	if (sem_init(&mutex_ready, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	if (sem_init(&mutex_exit, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		exit (EXIT_FAILURE);
	}
	return;
}

int configurar_senales_planificador(void) 
{
    // Declara e inicializa en 0 la estructura 'accion' para configurar el comportamiento ante señales.
    // Esto asegura que todos los campos de control internos de la estructura arranquen limpios.
    struct sigaction accion = {0}; 

    // Asigna el puntero a la función 'manejar_sigint_planificador' para que actúe como la rutina 
    // de callback que se ejecutará asincrónicamente cuando el proceso intercepte la señal elegida.
    accion.sa_handler = manejar_sigint_planificador;

    // Inicializa el conjunto de señales sa_mask dejándolo completamente vacío.
    // Esto garantiza que, mientras se esté ejecutando el manejador, no se bloquee ninguna otra señal del sistema.
    sigemptyset(&accion.sa_mask);

    // Configura los flags sa_flags en 0 para indicarle al kernel de Linux que utilice el comportamiento 
    // por defecto al despachar la señal, sin activar modificadores especiales (como SA_RESTART o SA_SIGINFO).
    accion.sa_flags = 0;
	
    // Registra la nueva configuración asociada a la señal SIGINT (provocada comúnmente con Ctrl+C en la terminal).
    // Si la llamada al sistema devuelve -1, significa que el sistema operativo rechazó la configuración.
    if (sigaction(SIGINT, &accion, NULL) == -1)
    {
        // Imprime en la salida de error estándar (stderr) el motivo exacto del fallo del sistema.
        perror("Error al configurar sigaction para SIGINT"); 
        
        // Finaliza de inmediato la ejecución del programa indicando un estado fallido al sistema operativo.
        exit(EXIT_FAILURE); 
    }

    return 0;
}

uint32_t generar_jid (void) 
{
    // Se inicializa en 0 SOLO la primera vez que se invoca la función.
    // En las siguientes llamadas, mantiene el último valor guardado.
    static uint32_t contador = 0; 
    
    // Retornamos el valor actual y post-incrementamos para la próxima llamada
    return contador++; 
}
