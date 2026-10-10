#include "configuracion_placa.h"

void cargar_configuracion_placa (t_config * configuracion)
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
	
	if (!config_has_property(configuracion, "TAM_MEMORIA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: TAM_MEMORIA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "TAM_PAGINA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: TAM_PAGINA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "RETARDO_MEMORIA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: RETARDO_MEMORIA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "ALGORITMO_REEMPLAZO")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: ALGORITMO_REEMPLAZO\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PATH_INSTRUCCIONES")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PATH_INSTRUCCIONES\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PATH_OFFLOAD")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PATH_OFFLOAD\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "TAM_OFFLOAD")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: TAM_OFFLOAD\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "RETARDO_OFFLOAD")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: RETARDO_OFFLOAD\n");
		exit(EXIT_FAILURE);
	}

	// Carga segura de los datos una vez que confirmamos que existen
	puerto_escucha = config_get_string_value (configuracion, "PUERTO_ESCUCHA");
	log_level = log_level_from_string (config_get_string_value(configuracion, "LOG_LEVEL"));
	tam_memoria = config_get_int_value (configuracion, "TAM_MEMORIA");
	tam_pagina = config_get_int_value (configuracion, "TAM_PAGINA");
	retardo_memoria = config_get_int_value (configuracion, "RETARDO_MEMORIA");
	algoritmo_reemplazo = config_get_string_value (configuracion, "ALGORITMO_REEMPLAZO");
	path_instrucciones = config_get_string_value (configuracion, "PATH_INSTRUCCIONES");
	path_offload = config_get_string_value (configuracion, "PATH_OFFLOAD");
	tam_offload = config_get_int_value (configuracion, "TAM_OFFLOAD");
	retardo_offload = config_get_int_value (configuracion, "RETARDO_OFFLOAD");
	
	return;
}

