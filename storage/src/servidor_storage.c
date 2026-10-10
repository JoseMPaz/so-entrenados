#include "servidor_storage.h"

void * servidor_storage (void * socket)
{
	int socket_escucha = *(int *)socket;
	int socket_aceptado;
    
	// Declaramos las variables una sola vez al principio para evitar errores de redefinición
	int * socket_de_atencion;
	t_paquete * paquete;
	pthread_t hilo_de_atencion_a_planificador;
	t_operacion operacion;
				
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		socket_aceptado = accept (socket_escucha, NULL, NULL);
    	
    	if (socket_aceptado >= 0)
   		{
   			operacion = recibir_operacion (socket_aceptado);
   			
   			switch (operacion)
   			{   				
   				case SOLICITUD_CONEXION_PLANIF_TO_STORAGE:
   					// 1. Contestar el Handshake primero
   					paquete = crear_paquete (RESPUESTA_CONEXION_STORAGE_TO_PLANIF_OK);
   					enviar_paquete (paquete, socket_aceptado);
   					destruir_paquete (paquete);

   					// 2. Lanzar hilo de atención al Planificador
   					socket_de_atencion = (int *) malloc (sizeof(int)); 
   					*socket_de_atencion = socket_aceptado;
   					pthread_create	(&hilo_de_atencion_a_planificador, NULL, servidor_storage_to_planificador, 
   									(void *) socket_de_atencion);
   					pthread_detach (hilo_de_atencion_a_planificador);
   					break;
   				
   				default:
   					paquete = crear_paquete (RESPUESTA_CONEXION_FALLIDA);
   					enviar_paquete (paquete, socket_aceptado);
   					destruir_paquete (paquete);
                    close(socket_aceptado); // Evitamos leaks de sockets
   					break;
   			}   			
   		}    	
	}

    printf("Hilo del servidor placa cerrándose de forma limpia...\n");
    pthread_exit(NULL);
}


void * servidor_storage_to_planificador (void * socket)
{
	t_operacion operacion;
	t_paquete * paquete;
	//t_list * carga_util = NULL;
	int socket_storage_to_planificador = *(int *)socket;
	//int jid;
	//char * path_instrucciones = NULL;
	
	if (socket != NULL)
		free (socket);
	
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST))
	{
		int operacion_recibida = recibir_operacion(socket_storage_to_planificador);

		// Si es -1, sabemos con total certeza que el Planificador se desconectó
		if (operacion_recibida == -1) 
		{
			printf("[Placa]: El Planificador se ha desconectado de forma limpia o abrupta.\n");
			break; // Rompe el while, sale del bucle y va directo al close(socket) final
		}

		operacion = (t_operacion)operacion_recibida;
		
		switch (operacion)
   		{
   			case PLANIF_TO_STORAGE_SAVE_CHECKPOINT:
   				/*recibir carga util: jid - path_instrucciones y cargarlo en lista instrucciones_de_jobs*/
   				//carga_util = recibir_carga_util (socket_placa_to_planificador);
   				//jid = (int) strtol (list_get (carga_util, JID), NULL, BASE_DECIMAL);
   				//path_instrucciones = list_get (carga_util, PATH_INSTRUCCIONES);
   				//list_add (instrucciones_de_jobs, crear_codigo_proceso (jid, path_instrucciones));
				//list_destroy_and_destroy_elements (carga_util, free);
				//list_iterate(instrucciones_de_jobs, imprimir_instrucciones);
   				/*Agregar a tablas de paginas el jid y lista vacia de tabla de paginas*/
   				
   				paquete = crear_paquete (OPERACION_OK);
				enviar_paquete (paquete, socket_storage_to_planificador);
				destruir_paquete (paquete);
   				break;
   			default:
   				paquete = crear_paquete (OPERACION_ERROR);
				enviar_paquete (paquete, socket_storage_to_planificador);
				destruir_paquete (paquete);
   		}
	}
	
	close (socket_storage_to_planificador);//No debe llegr aca ya que es un servidor
	
	pthread_exit(NULL);
}
