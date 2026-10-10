#include "configuracion_storage.h"

void cargar_configuracion_storage (t_config * configuracion)
{
	// Validación individual de cada clave antes de cargar
	if (!config_has_property(configuracion, "PUERTO_ESCUCHA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PUERTO_ESCUCHA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "LOG_LEVEL")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: LOG_LEVEL\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PATH_STORAGE")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PATH_STORAGE\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "CANT_BLOQUES")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: CANT_BLOQUES\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "TAM_BLOQUE")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: TAM_BLOQUE\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "BLOQUES_DIRECTORIO")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: BLOQUES_DIRECTORIO\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "RETARDO_ACCESO_BLOQUE")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: RETARDO_ACCESO_BLOQUE\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "RETARDO_COMMIT")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: RETARDO_COMMIT\n");
		exit(EXIT_FAILURE);
	}
	
	puerto_escucha = config_get_string_value (configuracion,"PUERTO_ESCUCHA");
	log_level = log_level_from_string ( config_get_string_value(configuracion,"LOG_LEVEL") );
	path_storage = config_get_string_value (configuracion,"PATH_STORAGE");
	cant_bloques = config_get_int_value (configuracion,"CANT_BLOQUES");
	tam_bloque = config_get_int_value (configuracion,"TAM_BLOQUE");
	bloques_directorio = config_get_int_value (configuracion,"BLOQUES_DIRECTORIO");
	retardo_acceso_bloque = config_get_int_value (configuracion,"RETARDO_ACCESO_BLOQUE");
	retardo_commit = config_get_int_value (configuracion,"RETARDO_COMMIT");
	
	return;
}
