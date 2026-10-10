#ifndef _ESTRUCTURAS_ADMINISTRATIVAS_H_
#define _ESTRUCTURAS_ADMINISTRATIVAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <stdbool.h>
#include <commons/bitarray.h>
#include <string.h>



extern char * path_instrucciones;
extern char * path_offload;
extern int tam_pagina;
extern t_bitarray* bitarray_offload;
extern uint32_t bloques_totales_offload;

/*Para las instrucciones*/
typedef struct
{
	uint32_t pid;
	t_list * instrucciones;
}t_codigo_proceso;


// Cada nodo que va a ir dentro del t_list*
typedef struct 
{
    uint32_t nro_marco;      // El marco físico asignado en la RAM (si presencia es true)
    bool bit_presencia;      // true si está en RAM, false si está en Offload (Page Fault)
    bool bit_modificado;     // true si fue escrita (Dirty bit, clave para CLOCK-M)
    bool bit_uso;            // true si fue accedida recientemente (clave para CLOCK-M)
    uint32_t pos_offload;    // Posición o bloque reservado en el archivo de Offload
} t_entrada_pagina;

// Estructura administrativa por cada Job en la Placa
typedef struct 
{
    uint32_t jid;            // Job ID
    t_list* tabla_paginas;   // Lista de (t_entrada_pagina *)
} t_control_job;

extern int tam_memoria;

t_codigo_proceso * crear_codigo_proceso (uint32_t pid, char * path_job);
void destruir_codigo_proceso (void *elemento);
void imprimir_instrucciones (void *elemento);
void * crear_memoria_de_datos (void);
FILE * crear_offload (void);
void destruir_offload (FILE * ptr_file);
uint32_t asignar_espacio_job(t_control_job* job, uint32_t bytes_a_asignar);
int obtener_bloque_libre_offload();
void inicializar_offload(uint32_t tam_offload, uint32_t tam_pagina);
void liberar_bloque_offload(uint32_t nro_bloque);

#endif

