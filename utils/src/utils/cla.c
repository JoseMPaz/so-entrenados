#include <utils/cla.h>

void validar_argumentos_placa (int argc, char * argv[])
{
	if (argc != CAMPO_EJECUTABLE + CAMPO_CONFIGURACION)
	{
		fprintf (stderr, "%s\n", MSJ_ERROR_UN_ARGUMENTO);
		exit (ERROR_CANTIDAD_DE_ARGUMENTOS);
	}
	validar_ruta_configuracion (argv[POSICION_RUTA_CONFIGURACION]);
	
	return;
}

void validar_un_argumento (int argc, char * argv[])
{
	if (argc != CAMPO_EJECUTABLE + CAMPO_CONFIGURACION)
	{
		fprintf (stderr, "%s\n", MSJ_ERROR_UN_ARGUMENTO);
		exit (ERROR_CANTIDAD_DE_ARGUMENTOS);
	}
	validar_ruta_configuracion (argv[POSICION_RUTA_CONFIGURACION]);
	
	return;
}

void validar_dos_argumento (int argc, char * argv[])
{
	if (argc != CAMPO_EJECUTABLE + CAMPO_CONFIGURACION + CAMPO_IDENTIFICADOR)
	{
		fprintf (stderr, "%s\n", MSJ_ERROR_DOS_ARGUMENTO);
		exit (ERROR_CANTIDAD_DE_ARGUMENTOS);
	}
	validar_ruta_configuracion (argv[POSICION_RUTA_CONFIGURACION]);
	
	return;
}

void validar_ruta_configuracion (char * ruta_config)
{
	FILE * ptr_config = NULL;
	
	ptr_config = fopen (ruta_config, "r");

	if (ptr_config == NULL)
	{
		fprintf (stderr, "%s\n", MSJ_ERROR_RUTA_CONFIGURACION);
		exit (ERROR_RUTA_CONFIGURACION);
	}
	fclose (ptr_config);
	
	return;
}




