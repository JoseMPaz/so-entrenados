#ifndef _PLANIFICADOR_LARGO_PLAZO_H_
#define _PLANIFICADOR_LARGO_PLAZO_H_

#include "estructuras_administrativas.h"
#include <pthread.h>
#include <signal.h>
#include <utils/historial.h>
#include <commons/temporal.h>

#define PRIMER_ELEMENTO 0

extern pthread_t hilo_ingreso_de_jobs;
extern pthread_t hilo_egreso_de_jobs;
extern volatile sig_atomic_t salir;
extern sem_t sem_grado_multiprogramacion;

extern sem_t sem_ingresar_job_a_new;
extern sem_t sem_ingresar_job_a_ready;
extern sem_t sem_ingresar_job_a_exit;

extern t_list * cola_de_new;
extern t_list * cola_de_ready;
extern t_list * cola_de_exit;

extern sem_t mutex_new;
extern sem_t mutex_ready;
extern sem_t mutex_exit;

extern t_log * historial;
extern t_temporal * reloj_sistema;

void * planificar_largo_plazo (void * arg);
void * mover_jobs_de_new_a_ready (void * arg);
void * eliminar_jobs_de_exit (void * arg);


#endif
