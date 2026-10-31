#ifndef _SERVIDOR_PLACA_H_
#define _SERVIDOR_PLACA_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <utils/conexion.h>
#include <utils/historial.h>
#include "estructuras_administrativas.h"

#define JID 0 //Posicion en la carga util
#define PATH_INSTRUCCIONES 1 //Posicion en la carga util
#define PROGRAM_COUNTER 1 //Posicion en la carga util
#define BASE_DECIMAL 10
#define ID_CORE 0
#define BYTES 0

extern volatile sig_atomic_t salir;
extern t_list * instrucciones_de_jobs;
extern t_list * tablas_de_paginas_de_jobs;
extern int jid_a_buscar; 
extern t_log * historial;
extern int cantidad_de_kernel;
extern int retardo_memoria;
extern int tam_pagina;

/*Esta funcion acepta conexiones del planificador y de los cores y 
delega su atencion a atender_planificador y atender core*/
void * servidor_placa (void * socket);
void * atender_peticiones_del_planificador (void * socket);
void * atender_peticiones_de_un_core (void * socket);
bool buscar_por_jid (void* elemento);

#endif
