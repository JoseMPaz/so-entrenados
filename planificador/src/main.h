#ifndef _MAIN_H_
#define _MAIN_H_

/*
	run:
		./bin/planificador planificador.config no_hacer_nada.asm
	run valgrind:
		valgrind --leak-check=full --show-leak-kinds=all ./bin/planificador planificador.config no_hacer_nada.asm
	run helgrind:
		valgrind --tool=helgrind ./bin/planificador planificador.config no_hacer_nada.asm
*/

#include <utils/hello.h>
#include <utils/cla.h>
#include <utils/historial.h>
#include <utils/conexion.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include <signal.h>
#include "configuracion_planificador.h"
#include "estructuras_administrativas.h"
#include "servidor_planificador.h"
#include "manejar_senales.h"
#include "planificador_largo_plazo.h"
#include "planificador_corto_plazo.h"


#define PATH_CONFIG 1

/* ========== Declaracion de Variables Globales ========== */

/*Para desescribir eventos en el modulo planificador*/
t_log * historial = NULL;// Log general del módulo (para auditoría y debugging)
t_log* log_reportes = NULL;    // Log exclusivo para los reportes de los Jobs

volatile sig_atomic_t salir = 0;// Definición de la bandera segura ante un CTR+C

int socket_planificador_to_placa;



// Tus estructuras globales sincronizadas
t_list * cores = NULL;

sem_t sem_core_disponible;
sem_t sem_grado_multiprogramacion;//Contador
sem_t sem_ingresar_job_a_new;
sem_t sem_ingresar_job_a_ready;
sem_t sem_ingresar_job_a_exit;

sem_t mutex_cores;
sem_t mutex_new;
sem_t mutex_ready;
sem_t mutex_exit;

/*Estructuras para gestion de jobs*/
t_list * cola_de_new = NULL;
t_list * cola_de_ready = NULL;
t_list * cola_de_exec = NULL;
t_list * cola_de_block = NULL;
t_list * cola_de_exit = NULL;

pthread_t hilo_servidor;
pthread_t hilo_ingreso_de_jobs;
pthread_t hilo_egreso_de_jobs;
int socket_escucha_global;

t_temporal * reloj_sistema;
t_scheduler planificar_corto_plazo;//Puntero a funcion

int socket_escucha = -1; // La declarás global e inicializada en -1

//Variables de configuración
char * puerto_escucha = NULL;
char * ip_placa = NULL;
char * puerto_placa = NULL;
char * ip_storage = NULL;
char * puerto_storage = NULL;
t_log_level log_level;
char * algoritmo_planificacion = NULL;
int rr_quantum;
int estimacion_inicial;
double hrrn_alfa;
int grado_multiprogramacion;
char * path_datos = NULL;
int retardo_loader;
char * path_reportes = NULL;

void inicializar_semaforos (void);
int configurar_senales_planificador(void);
uint32_t generar_jid (void);

#endif
