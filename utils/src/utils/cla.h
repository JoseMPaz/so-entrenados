#ifndef _CLA_H_
#define _CLA_H_

#include <stdlib.h>
#include <stdio.h>

#define CAMPO_EJECUTABLE 1
#define CAMPO_CONFIGURACION 1
#define CAMPO_IDENTIFICADOR 1
#define MSJ_ERROR_UN_ARGUMENTO "Error: Se debe ingresar un argumento por linea de comandos"
#define MSJ_ERROR_DOS_ARGUMENTO "Error: Se debe ingresar dos argumento por linea de comandos"
#define MSJ_ERROR_RUTA_CONFIGURACION "Error: No se puede abrir el archivo de configuracion"
#define ERROR_CANTIDAD_DE_ARGUMENTOS 1
#define ERROR_RUTA_CONFIGURACION 2
#define POSICION_RUTA_CONFIGURACION 1
#define POSICION_IDENTIFICADOR 2

typedef void (*t_validar_argumentos) (int argc, char * argv[]);

void validar_argumentos_placa (int argc, char * argv[]);
void validar_un_argumento (int argc, char * argv[]);
void validar_dos_argumento (int argc, char * argv[]);
void validar_ruta_configuracion (char * ruta_config);


#endif
