#ifndef _CONSOLA_H_
#define _CONSOLA_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <utils/conexion.h>

#define TAM_LEIDO 100

extern volatile sig_atomic_t salir;

typedef enum
{
	INFO,
	TLS
}t_opcion_menu;

void * consola (void * arg);
void mostrar_consola (void);
char * leer_consola (void);
bool hacer_consola(char *leido);
void manejar_sigint(int sig); 

#endif
