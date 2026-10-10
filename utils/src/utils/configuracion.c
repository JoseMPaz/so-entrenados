#include "configuracion.h"

t_config * crear_configuracion (char * ruta_archivo)
{
	t_config * nuevo_config = NULL;

	if ( (nuevo_config = config_create(ruta_archivo) ) == NULL)
	{
		printf("No se pudo leer el config \n");
		exit(EXIT_FAILURE);
	}

	return nuevo_config;
}

void destruir_configuracion (t_config * configuracion)
{
	config_destroy (configuracion);
	return;
}
