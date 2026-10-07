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

void cargar_configuracion_placa (t_config * configuracion)
{
	puerto_escucha = config_get_string_value (configuracion,"PUERTO_ESCUCHA");
	log_level = log_level_from_string ( config_get_string_value(configuracion,"LOG_LEVEL") );
	tam_memoria = config_get_int_value (configuracion,"TAM_MEMORIA");
	tam_pagina = config_get_int_value (configuracion,"TAM_PAGINA");
	retardo_memoria = config_get_int_value (configuracion,"RETARDO_MEMORIA");
	algoritmo_reemplazo = config_get_string_value (configuracion,"ALGORITMO_REEMPLAZO");
	path_instrucciones = config_get_string_value (configuracion,"PATH_INSTRUCCIONES");
	path_offload = config_get_string_value (configuracion,"PATH_OFFLOAD");
	tam_offload = config_get_int_value (configuracion,"TAM_OFFLOAD");
	retardo_offload = config_get_int_value (configuracion,"RETARDO_OFFLOAD");
	
	return;
}

void destruir_configuracion (t_config * configuracion)
{
	config_destroy (configuracion);
	return;
}



