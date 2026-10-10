#ifndef _CONFIGURACION_STORAGE_H_
#define _CONFIGURACION_STORAGE_H_
#include <utils/configuracion.h>

extern char * puerto_escucha;
extern t_log_level log_level;
extern char * path_storage;
extern int cant_bloques;
extern int tam_bloque;
extern int bloques_directorio;
extern int retardo_acceso_bloque;
extern int retardo_commit;

void cargar_configuracion_storage (t_config * configuracion);

#endif
