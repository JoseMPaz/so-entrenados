#include "main.h"

/*
Compilar: make
Ejecutar: ./bin/planificador planificador.config no_hacer_nada.asm
*/

int main(int argc, char * argv[]) 
{
	struct sigaction accion = {0}; // Inicializa la estructura 'accion' llenando todos sus campos con 0.
	accion.sa_handler = manejar_sigint_planificador;
	sigemptyset(&accion.sa_mask);
	accion.sa_flags = 0;
	
	if (sigaction(SIGINT, &accion, NULL) == -1)
	{
		perror("sigaction"); // Imprime en la salida de error estándar (stderr) el motivo del fallo
		return EXIT_FAILURE; // Finaliza el programa indicando un estado de error al sistema operativo
	}
	
	/* Inicio del Modulo Planificador */
	t_validar_argumentos validar_argumentos = validar_argumentos_planificador; // Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_planificador; // Setea la funcion cargar_configuracion
	t_config * configuracion = NULL;
	t_paquete * paquete;
	t_operacion operacion;
	int socket_escucha;
	pthread_t hilo_planificador_largo_plazo;
	// pthread_t hilo_planificador_corto_plazo;
	
	// 1. En tu main.c creás el reloj base del sistema:
	reloj_sistema = temporal_create();
	
	validar_argumentos (argc, argv);
	configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	
	historial = crear_historial ("planificador.log", "PLANIFICADOR_LOG", log_level); 
	
	cores = list_create (); // Esta lista permite administrar los cores que se tiene a disposicion para ejecutar los Jobs
	cola_de_new = list_create ();
	cola_de_ready = list_create ();
	cola_de_exec = list_create ();
	cola_de_block = list_create ();
	cola_de_exit = list_create ();
	
	establecer_planificador_corto_plazo (algoritmo_planificacion);
	
	if (sem_init(&mutex_cores, 0, 1 /* Protege un recurso lista */) != 0) 
	{
		perror("Error al inicializar el semáforo mutex_cores");
		return EXIT_FAILURE;
	}
	
	if (sem_init(&sem_core_disponible, 0, 0 /* Inicia sin cores disponibles */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	
	if (sem_init(&sem_grado_multiprogramacion, 0, grado_multiprogramacion) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	
	if (sem_init(&sem_ingresar_job_a_new, 0, 0 /* Indica el ingreso a la cola de new */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	if (sem_init(&sem_ingresar_job_a_ready, 0, 0 /* Pausa para pasar jobs de cola_de_ready */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	if (sem_init(&sem_ingresar_job_a_exit, 0, 0 /* Pausa para destruir jobs de cola_de_exit */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	if (sem_init(&mutex_new, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	if (sem_init(&mutex_ready, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	if (sem_init(&mutex_exit, 0, 1 /* Por ser mutex */) != 0) 
	{
		perror("Error al inicializar el semáforo sem_core_disponible");
		return EXIT_FAILURE;
	}
	
	/* == Inicio Handshake Planificador - Placa == */
	socket_planificador_to_placa = crear_socket (CLIENTE, ip_placa, puerto_placa);
	conectar_a_servidor (socket_planificador_to_placa, ip_placa, puerto_placa);
	log_info (historial, "%s%s", "Solicitud de conexión: ", "## Módulo: Placa");
	paquete = crear_paquete (SOLICITUD_CONEXION_PLANIF_TO_PLACA);
	enviar_paquete (paquete, socket_planificador_to_placa);
	destruir_paquete (paquete);
	operacion = recibir_operacion (socket_planificador_to_placa);
	printf ("HOla1\n");
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
	printf ("HOla2\n");
	
	/*Servidor para aceptar conexiones de core*/
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);
	socket_escucha_global = socket_escucha;
	
	printf ("HOla3\n");
	// 🔥 CORRECCIÓN: Pasamos por referencia la variable global para asegurar que close() impacte de inmediato
	pthread_create (&hilo_servidor, NULL, servidor_planificador, (void *) &socket_escucha_global);
	
	printf ("HOla4\n");
	pthread_create (&hilo_planificador_largo_plazo, NULL, planificar_largo_plazo, NULL);
	
	printf ("HOla5\n");
	// pthread_create (&hilo_planificador_corto_plazo, NULL, planificar_corto_plazo, NULL);
	printf ("HOla6\n");
	
	printf ("HOla7\n");
	pthread_join (hilo_servidor, NULL);
	printf ("HOla8\n");
	pthread_join (hilo_planificador_largo_plazo, NULL);
	printf ("HOla9\n");
	// pthread_join (hilo_planificador_corto_plazo, NULL);
	printf ("HOla10\n");
	
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

