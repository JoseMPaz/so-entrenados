#ifndef _ESTRUCTURAS_ADMINISTRATIVAS_H_
#define _ESTRUCTURAS_ADMINISTRATIVAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>


extern char * path_instrucciones;

typedef struct
{
	uint32_t pid;
	t_list * instrucciones;
}t_codigo_proceso;

extern int tam_memoria;

t_codigo_proceso * crear_codigo_proceso (uint32_t pid, char * path_job);
void destruir_codigo_proceso (void *elemento);
void imprimir_instrucciones (void *elemento);
void * crear_memoria_de_datos (void);


#endif

