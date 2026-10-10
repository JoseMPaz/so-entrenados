#ifndef _CONFIGURACION_H_
#define _CONFIGURACION_H_

#include <commons/config.h>
#include <commons/log.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern char * puerto_escucha;
extern t_log_level log_level;
extern int tam_memoria;
extern int tam_pagina;
extern int retardo_memoria;
extern char * algoritmo_reemplazo;
extern char * path_instrucciones;
extern char * path_offload;
extern int tam_offload;
extern int retardo_offload;

typedef void (*t_cargar_configuracion) (t_config * configuracion);

t_config * crear_configuracion (char * ruta_archivo);
void destruir_configuracion (t_config * configuracion);

#endif
