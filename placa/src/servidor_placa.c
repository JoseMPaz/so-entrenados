#include "servidor_placa.h"

void * servidor_placa (void * socket)
{
	int socket_escucha = *(int *)socket;
			
	printf ("%d\n", socket_escucha);		
	// Bucle de control principal que se ejecuta continuamente mientras no se ordene la salida.
	// __atomic_load_n lee el valor de 'salir' de manera completamente atómica.
	// __ATOMIC_SEQ_CST actúa como una barrera de memoria (memory barrier), obligando al procesador
	// a leer directamente el valor real actualizado por el Signal Handler en lugar de usar copias en caché.
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
    // Escuchar conexiones, procesar datos de la placa, etc.
    // El hilo realizará sus tareas normales de forma segura y concurrente acá adentro.
    	
	}


    printf("Hilo del servidor cerrándose de forma limpia...\n");
    return NULL;
}

