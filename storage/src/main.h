#ifndef _MAIN_H_
#define _MAIN_H_

/*
	run:
		./bin/storage storage.config
	run valgrind:
		valgrind --leak-check=full --show-leak-kinds=all ./bin/storage storage.config
	run helgrind:
		valgrind --tool=helgrind ./bin/storage storage.config
*/

#include <utils/hello.h>
#include <utils/cla.h>
#include <utils/historial.h>
#include "configuracion_storage.h"
#include "servidor_storage.h"
#include <utils/conexion.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <signal.h>

#define PATH_CONFIG 1

/* ========== Declaracion de Variables Globales ========== */

/*Para desescribir eventos en el modulo placa*/
t_log * historial = NULL;

volatile sig_atomic_t salir = 0;// Definición de la bandera segura ante un CTR+C

//Variables de configuración
char * puerto_escucha;
t_log_level log_level;
char * path_storage;
int cant_bloques;
int tam_bloque;
int bloques_directorio;
int retardo_acceso_bloque;
int retardo_commit;

#endif
