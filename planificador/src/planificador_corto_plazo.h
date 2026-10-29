#ifndef _PLANIFICADOR_CORTO_PLAZO_H_
#define _PLANIFICADOR_CORTO_PLAZO_H_

#include "estructuras_administrativas.h"
#include <pthread.h>
#include <string.h>
#include <utils/conexion.h>
#include <utils/operaciones.h>
#include <signal.h>
#include <utils/historial.h>
#include <commons/temporal.h>

#define PRIMER_ELEMENTO 0

// Definimos la estructura temporal empaquetada para el envío de red sin problemas de alineación
typedef struct __attribute__((packed)) 
{
    uint32_t jid;
    t_registros_core contexto;
} t_payload_despacho;

// Estructura auxiliar para pasarle los datos al hilo
typedef struct 
{
    t_core * core; 
    t_jcb * job;
} t_args_despacho;

typedef void * (*t_scheduler) (void * arg);

extern sem_t sem_core_disponible;

extern sem_t sem_pasaje_de_job_a_ready;
extern sem_t sem_pasaje_de_job_a_exit;

extern t_scheduler planificar_corto_plazo;
extern t_list * cola_de_new;
extern t_list * cola_de_ready;
extern t_list * cola_de_exit;

extern sem_t mutex_new;
extern sem_t mutex_ready;
extern sem_t mutex_exit;

extern t_log * historial;
extern t_temporal * reloj_sistema;

void * planificar_por_fifo (void * arg);
/*
bool _es_libre(void * elemento);
t_core * buscar_primer_core_libre(t_list * lista_cores);
void despachar_contexto_a_core(int socket_core, t_jcb *job);
void* ejecutar_job_en_core (void* void_args);*/
void * planificar_por_rr (void * arg);
void * planificar_por_hrrn (void * arg);

void establecer_planificador_corto_plazo (char * algoritmo);

#endif
