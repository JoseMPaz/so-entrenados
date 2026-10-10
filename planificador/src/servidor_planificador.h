#ifndef _SERVIDOR_PLANIFICADOR_H_
#define _SERVIDOR_PLANIFICADOR_H_

#include <stdio.h>
#include <stdlib.h>
#include <utils/conexion.h>
#include "estructuras_administrativas.h"

#define BASE_DECIMAL 10
#define ID_CORE 0 //Posicion en la carga util

extern t_list * cores;
extern sem_t sem_core_disponible;

void * servidor_planificador (void * socket);

#endif
