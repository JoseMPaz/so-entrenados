#ifndef _CONSOLA_H_
#define _CONSOLA_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <utils/conexion.h>
#include <commons/string.h>
#include "estructuras_administrativas.h"

#define TAM_LEIDO 100

extern int marcos_totales;
extern int tam_memoria;
extern int tam_marco;
extern t_marco_control * vector_de_marcos_memoria;
extern t_bitarray * bitmap_offload;
extern int cantidad_bloques_totales; 
extern int cantidad_bloques_totales; 
extern int tam_bloque;
extern int tam_offload;
extern int tam_pagina;
extern t_list * instrucciones_de_jobs;

typedef enum
{
	INFO,
	TLS
}t_opcion_menu;

void * consola (void * arg);
void mostrar_consola (void);
char * leer_consola (void);
bool hacer_consola(char *leido);

#endif
