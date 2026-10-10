#ifndef _CONFIGURACION_PLANIFICADOR_H_
#define _CONFIGURACION_PLANIFICADOR_H_

#include <utils/configuracion.h>

extern char * puerto_escucha;
extern char * ip_placa;
extern char * puerto_placa;
extern char * ip_storage;
extern char * puerto_storage;
extern t_log_level log_level;
extern char * algoritmo_planificacion;
extern int rr_quantum;
extern int estimacion_inicial;
extern double hrrn_alfa;
extern int grado_multiprogramacion;
extern char * path_datos;
extern int retardo_loader;
extern char * path_reportes;

void cargar_configuracion_planificador (t_config * configuracion);

#endif
