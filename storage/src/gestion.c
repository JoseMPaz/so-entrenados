#include "gestion.h"

/*
void * procesar_peticiones_storage (void * socket_escucha)
{
	int socket_temporal;
	int socket = *(int*) socket_escucha;
	free(socket_escucha);  // liberar memoria
	
	while(true)
	{
   		pthread_t hilo_de_atencion; //Por cada solicitud de atencion genera un hilo nuevo  	
    	
   		socket_temporal = accept (socket, NULL, NULL);//Se queda aca esperando peticiones, ya que accept es bloqueante
    	
   		if (socket_temporal >= 0)
   		{
   			//La funcion dentro del hilo_de_atencion debe liberar esta peticion de memoria
   			int * socket_de_atencion = (int *) malloc (sizeof(int)); 
    		
   			*socket_de_atencion = socket_temporal;
    		
   			pthread_create (&hilo_de_atencion, NULL, procesar_peticion_storage, (void *) socket_de_atencion);
   			pthread_detach (hilo_de_atencion);
   		}   	
  	}
	pthread_exit(NULL);

	return NULL;
}
				
void * procesar_peticion_storage (void * socket_de_atencion)
{
	if (socket_de_atencion != NULL)
		free (socket_de_atencion);
	return NULL;
}
*/
