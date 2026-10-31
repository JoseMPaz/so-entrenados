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
	configurar_senales_placa ();
	
	t_validar_argumentos validar_argumentos = validar_argumentos_placa;//Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_placa;//Setea la funcion cargar_configuracion
	
	
	validar_argumentos (argc, argv);//Valida los Argumentos ingresados por la Linea de Comando al ejecutar el modulo
	t_config * configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	historial = crear_historial ( "placa.log", "PLACA_LOG", log_level); 
	
	log_info (historial, "Inicio del Módulo Placa");// LOG 1: Optativo
	
	instrucciones_de_jobs = list_create ();	/*Estructura administrativa, sus nodos tienen id e instrucciones de los job*/
	tablas_de_paginas_de_jobs = list_create ();
	memoria_de_datos = crear_memoria_de_datos ();/*Espacio  de trabajo de los Jobs (void *) */
	offload = crear_offload (tam_offload);
	cantidad_bloques_totales = tam_offload / tam_pagina; 
	printf ("%s%d\n","Cantidad de bloques totales en offload", cantidad_bloques_totales);
	//Regla de tres simples: 8[b] --- 1[B], cantidad_bloques_totales [bloques] * 1[b]/1[bloque] --- tamano_en_bytes[B]
	size_t tamano_en_bytes = cantidad_bloques_totales / 8; 
	char* estructura_bits = malloc(tamano_en_bytes);
	if (estructura_bits == NULL) 
	{
		fprintf (stderr,"%s\n","Error al crear estructura de bitmap");
    	return EXIT_FAILURE; 
	}
	// Todos los bloques se marcan como libre, o sea sus bits en 0
	memset(estructura_bits, 0, tamano_en_bytes);
	bitmap_offload = bitarray_create_with_mode (estructura_bits, tamano_en_bytes, LSB_FIRST);
	tam_marco = tam_pagina;
	tam_bloque = tam_pagina;
	marcos_totales = tam_memoria / tam_marco;
	
	vector_de_marcos_memoria = (t_marco_control *) malloc (sizeof(t_marco_control) * marcos_totales);
	
	if (vector_de_marcos_memoria == NULL) 
	{
		fprintf(stderr, "Error: No se pudo asignar memoria para el vector de marcos de la memoria de datos.\n");
		return EXIT_FAILURE; 
	}
	/*Se inicializan libres los marcos de la memoria de datos*/
	for (int i = 0; i < marcos_totales; i++) 
	{
		vector_de_marcos_memoria[i].numero_de_marco = i;
		vector_de_marcos_memoria[i].esta_libre = true;     // Todos arrancan libres
		vector_de_marcos_memoria[i].esta_lockeado = false; //Todo arrancas deslockeados 
		vector_de_marcos_memoria[i].jid_dueno = 0;          // Inicializado en 0 por el tipo de dato
		vector_de_marcos_memoria[i].nro_pagina_dueno = -1;  // -1 indica que no contiene ninguna página válida
	}
	
	
	/*Inicio Prueba de memoria de instrucciones*/
	/*list_add (instrucciones_de_jobs, crear_codigo_proceso (7, "no_hacer_nada.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (15, "setear_registros_proposito_general.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (3, "suma_resta.asm"));*/
	
	//list_iterate(instrucciones_de_jobs, imprimir_instrucciones);
	/*Fin Prueba de memoria de instrucciones*/
	
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);	
	
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
	free (vector_de_marcos_memoria);
		
    saludar("placa");
    
    return 0;
}

int configurar_senales_placa (void) 
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

	return 0;
}


