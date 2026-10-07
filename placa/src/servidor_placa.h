#ifndef _SERVIDOR_PLACA_H_
#define _SERVIDOR_PLACA_H_

#include <stdio.h>
#include <stdlib.h>
#include <utils/conexion.h>

extern volatile sig_atomic_t salir;

/*Esta funcion acepta conexiones del planificador y de los cores y 
delega su atencion a atender_planificador y atender core*/
void * servidor_placa (void * socket);

#endif
