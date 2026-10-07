#include "historial.h"

t_log * crear_historial (char * archivo_log, char * etiqueta_log, t_log_level level)
{
	t_log * nuevo_log = log_create (archivo_log, etiqueta_log, true, level);
	
	if (nuevo_log == NULL)
	{
		log_error (nuevo_log, "%s", "No se pudo leer el log");
		exit (EXIT_FAILURE);
	}
		
	return nuevo_log;
}

void destruir_historial (t_log * log)
{
	log_destroy (log);
	return;
}
