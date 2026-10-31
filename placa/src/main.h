#ifndef _MAIN_H_
#define _MAIN_H_

/*
	run:
		./bin/placa placa.config
	run valgrind:
		valgrind --leak-check=full --show-leak-kinds=all ./bin/placa placa.config
	run helgrind:
		valgrind --tool=helgrind ./bin/placa placa.config
*/

#include <utils/hello.h>
#include <utils/cla.h>
#include <utils/historial.h>
#include <utils/conexion.h>
#include <commons/bitarray.h>
#include "configuracion_placa.h"
#include <commons/collections/list.h>
#include "estructuras_administrativas.h"
#include "consola.h"
#include "servidor_placa.h"
#include <pthread.h>
#include <signal.h>
#include "manejar_senales.h"
#include "manejar_senales.h"

#define PATH_CONFIG 1

/* ========== Declaracion de Variables Globales ========== */

/*Para desescribir eventos en el modulo placa*/
t_log * historial = NULL;

/*Memoria de Instrucciones*/
t_list * instrucciones_de_jobs = NULL;

t_list * tablas_de_paginas_de_jobs = NULL;

/*Memoria de Usuario*/
void * memoria_de_datos = NULL;
//Tablas de Paginas: Lista con nodos pid-tabla de pagina

/*Offload*/
FILE * offload = NULL;

volatile sig_atomic_t salir = 0;// Definición de la bandera segura ante un CTR+C

int jid_a_buscar; 

t_bitarray* bitarray_offload;
uint32_t bloques_totales_offload;

pthread_t hilo_consola;
pthread_t hilo_servidor;
int cantidad_de_kernel = 0;
int socket_escucha = -1;
int cantidad_bloques_totales; 
t_bitarray * bitmap_offload;
int tam_marco;
int marcos_totales;
t_marco_control * vector_de_marcos_memoria;
int tam_bloque;



//Variables de configuración
char * puerto_escucha;
t_log_level log_level;
int tam_memoria;
int tam_pagina;
int retardo_memoria;
char * algoritmo_reemplazo;
char * path_instrucciones;
char * path_offload;
int tam_offload;
int retardo_offload;

int configurar_senales_placa (void);

#endif
