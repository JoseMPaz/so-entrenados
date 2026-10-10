#include "servidor_planificador.h"

void * servidor_planificador (void * socket)
{
	int socket_escucha = *(int *)socket;
	int socket_aceptado;
	t_list * carga_util = NULL;
	// Declaramos las variables una sola vez al principio para evitar errores de redefinición
	t_paquete * paquete;
	t_operacion operacion;
	int id_core;
	char * ptr_ultimo_caracter;
				
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		socket_aceptado = accept (socket_escucha, NULL, NULL);//Bloqueante hasta que se conecte un nuevo core
    	
    	if (socket_aceptado >= 0)
   		{
   			operacion = recibir_operacion (socket_aceptado);
   			
   			switch (operacion)
   			{
   				/*El Core se conecta y luego queda a la espera de solicitudes del planificador*/
   				case SOLICITUD_CONEXION_CORE_TO_PLANIF:
   					carga_util = recibir_carga_util (socket_aceptado);
   					id_core = (int) strtol ((char *) list_get (carga_util, ID_CORE) , &ptr_ultimo_caracter, BASE_DECIMAL);
   					list_destroy_and_destroy_elements (carga_util, free);
   					printf ("%s%d\n", "ID CORE: ", id_core);
   					/*Cre el nodo para guardar el numero de socket*/
   					t_core * nuevo_core = (t_core *) malloc (sizeof(t_core));
   					//Setea los datos que son necesarios
   					nuevo_core->id_core = id_core;
   					nuevo_core->socket = socket_aceptado;
   					nuevo_core->estado = CORE_LIBRE;
   					list_add (cores, nuevo_core);
   					//Debo hacer un post para incrementar un semaforo para que el planificador de corto plazo
   					//pueda avanzar en mandar a ejecutar un job en ready a exec e iniciar la ejecucion
   					sem_post (&sem_core_disponible); 
   					// 1. Contestar el Handshake primero para asegurar el protocolo de red
   					paquete = crear_paquete (RESPUESTA_CONEXION_PLACA_TO_PLANIF_OK);
   					enviar_paquete (paquete, socket_aceptado);
   					destruir_paquete (paquete);
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


