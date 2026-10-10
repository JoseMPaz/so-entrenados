#include "configuracion_planificador.h"

#include "configuracion_planificador.h"

void cargar_configuracion_planificador (t_config * configuracion)
{
	// 1. Validación de propiedades globales básicas
	if (!config_has_property(configuracion, "PUERTO_ESCUCHA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PUERTO_ESCUCHA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "IP_PLACA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: IP_PLACA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PUERTO_PLACA")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PUERTO_PLACA\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "IP_STORAGE")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: IP_STORAGE\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PUERTO_STORAGE")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PUERTO_STORAGE\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "LOG_LEVEL")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: LOG_LEVEL\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "ALGORITMO_PLANIFICACION")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: ALGORITMO_PLANIFICACION\n");
		exit(EXIT_FAILURE);
	}

	// 2. Validación de las propiedades globales restantes
	if (!config_has_property(configuracion, "GRADO_MULTIPROGRAMACION")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: GRADO_MULTIPROGRAMACION\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PATH_DATOS")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PATH_DATOS\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "RETARDO_LOADER")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: RETARDO_LOADER\n");
		exit(EXIT_FAILURE);
	}
	
	if (!config_has_property(configuracion, "PATH_REPORTES")) 
	{
		fprintf(stderr, "[ERROR CONFIG] Falta la propiedad: PATH_REPORTES\n");
		exit(EXIT_FAILURE);
	}

	// 3. Carga inicial de los parámetros de control del algoritmo
	algoritmo_planificacion = config_get_string_value (configuracion, "ALGORITMO_PLANIFICACION");

	// 4. Validaciones y cargas condicionales según el algoritmo activo
	if (strcmp(algoritmo_planificacion, "RR") == 0)
	{
		if (!config_has_property(configuracion, "RR_QUANTUM")) 
		{
			fprintf(stderr, "[ERROR CONFIG] Algoritmo RR requiere la propiedad: RR_QUANTUM\n");
			exit(EXIT_FAILURE);
		}
		
		rr_quantum = config_get_int_value (configuracion, "RR_QUANTUM");
	}
	
	if (strcmp(algoritmo_planificacion, "HRRN") == 0)
	{
		if (!config_has_property(configuracion, "ESTIMACION_INICIAL")) 
		{
			fprintf(stderr, "[ERROR CONFIG] Algoritmo HRRN requiere la propiedad: ESTIMACION_INICIAL\n");
			exit(EXIT_FAILURE);
		}
		
		if (!config_has_property(configuracion, "HRRN_ALFA")) 
		{
			fprintf(stderr, "[ERROR CONFIG] Algoritmo HRRN requiere la propiedad: HRRN_ALFA\n");
			exit(EXIT_FAILURE);
		}
		
		estimacion_inicial = config_get_int_value (configuracion, "ESTIMACION_INICIAL");
		hrrn_alfa = config_get_double_value (configuracion, "HRRN_ALFA");
	}

	// 5. Carga segura de los datos globales restantes
	puerto_escucha = config_get_string_value (configuracion, "PUERTO_ESCUCHA");
	ip_placa = config_get_string_value (configuracion, "IP_PLACA");
	puerto_placa = config_get_string_value (configuracion, "PUERTO_PLACA");
	ip_storage = config_get_string_value (configuracion, "IP_STORAGE");
	puerto_storage = config_get_string_value (configuracion, "PUERTO_STORAGE");
	log_level = log_level_from_string (config_get_string_value(configuracion, "LOG_LEVEL"));
	grado_multiprogramacion = config_get_int_value (configuracion, "GRADO_MULTIPROGRAMACION");
	path_datos = config_get_string_value (configuracion, "PATH_DATOS");
	retardo_loader = config_get_int_value (configuracion, "RETARDO_LOADER");
	path_reportes = config_get_string_value (configuracion, "PATH_REPORTES");
	
	return;
}

