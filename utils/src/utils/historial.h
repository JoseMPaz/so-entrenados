#ifndef _HISTORIAL_H_
#define _HISTORIAL_H_

#include <commons/log.h>
#include <stdlib.h>

t_log * crear_historial (char * archivo_log, char * etiqueta_log, t_log_level level);
void destruir_historial (t_log * log);

#endif
