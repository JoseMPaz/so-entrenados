#include "main.h"

/*
Compilar: make
Ejecutar: ./bin/placa placa.config
*/

/* Teoria
Cantidad de Marcos(memoria_de_datos) = tam_memoria / tam_pagina
un Job puede tener muchas más páginas que "Cantidad de Marcos(memoria_de_datos)"
*/

int main (int argc, char * argv[]) 
{
	struct sigaction accion = {0};// Inicializa la estructura 'accion' llenando todos sus campos con 0.

	// Asigna la función 'manejar_sigint' como el manejador (handler) de la señal.
	// Esta es la función que se ejecutará automáticamente cuando llegue la señal.
	accion.sa_handler = manejar_sigint_placa;
	// Inicializa y vacía el conjunto de señales 'sa_mask'.
	// Durante la ejecución del handler, no se bloqueará ninguna otra señal adicional.
	sigemptyset(&accion.sa_mask);
	// Configura las flags en 0. Esto desactiva comportamientos especiales y usa
	// el comportamiento predeterminado del sistema operativo para esta señal.
	accion.sa_flags = 0;
	
	// Registra la nueva configuración asociando la señal SIGINT (Ctrl+C) con 'accion'.
	// Si la llamada al sistema devuelve -1, significa que hubo un error en el registro.
	if (sigaction(SIGINT, &accion, NULL) == -1)
	{
    	perror("sigaction");// Imprime en la salida de error estándar (stderr) el motivo del fallo
    	return EXIT_FAILURE; // Finaliza el programa indicando un estado de error al sistema operativo
	}
	
	t_validar_argumentos validar_argumentos = validar_argumentos_placa;//Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_placa;//Setea la funcion cargar_configuracion
	t_config * configuracion = NULL;
	int socket_escucha;
	
	validar_argumentos (argc, argv);//Valida los Argumentos ingresados por la Linea de Comando al ejecutar el modulo
	configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	historial = crear_historial ( "placa.log", "PLACA_LOG", log_level); 
	
	log_info (historial, "Inicio del Módulo Placa");// LOG 1: Optativo
	
	instrucciones_de_jobs = list_create ();	/*Estructura administrativa, sus nodos tienen pid e instrucciones*/
	memoria_de_datos = crear_memoria_de_datos ();/*Espacio  de trabajo de los Jobs (void *) */
	offload = crear_offload ();
	
	/*Inicio Prueba de memoria de instrucciones*/
	list_add (instrucciones_de_jobs, crear_codigo_proceso (7, "no_hacer_nada.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (15, "setear_registros_proposito_general.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (3, "suma_resta.asm"));
	
	//list_iterate(instrucciones_de_jobs, imprimir_instrucciones);
	/*Fin Prueba de memoria de instrucciones*/
	
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);	
	socket_escucha_global = socket_escucha;
	
	pthread_create (&hilo_consola, NULL, consola, NULL);
	pthread_create (&hilo_servidor, NULL, servidor_placa, (void *) &socket_escucha);
	
	pthread_join (hilo_consola, NULL);
	
	close(socket_escucha);
	pthread_cancel(hilo_servidor);
	
	pthread_join (hilo_servidor, NULL);
	
	
	/*Acá no deberia llegar ya que servidor_placa nunca finaliza su ejecución*/
	/*Destruir*/
	list_destroy_and_destroy_elements (instrucciones_de_jobs, destruir_codigo_proceso);
	free (memoria_de_datos);
	
	destruir_historial (historial);
	destruir_configuracion (configuracion);
	destruir_offload (offload);
		
    saludar("placa");
    
    return 0;
}


