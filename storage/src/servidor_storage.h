#ifndef _SERVIDOR_STORAGE_H
#define _SERVIDOR_STORAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <utils/conexion.h>
//#include "estructuras_administrativas.h"

#define JID 0 //Posicion en la carga util
#define PATH_INSTRUCCIONES 1 //Posicion en la carga util
#define PROGRAM_COUNTER 1 //Posicion en la carga util
#define BASE_DECIMAL 10

extern volatile sig_atomic_t salir;

/*Esta funcion acepta conexiones del planificador y de los cores y 
delega su atencion a atender_planificador y atender core*/
void * servidor_storage (void * socket);
void * servidor_storage_to_planificador (void * socket);


#endif
