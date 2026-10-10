#ifndef _CONFIGURACION_PLACA_H_
#define _CONFIGURACION_PLACA_H_

#include <utils/configuracion.h>

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

void cargar_configuracion_placa (t_config * configuracion);

#endif
