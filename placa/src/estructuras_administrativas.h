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
#include <commons/bitarray.h>

/*Para las instrucciones*/
typedef struct
{
	uint32_t jid;
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


typedef struct 
{
    uint32_t marco;
    unsigned int presencia;
    unsigned int uso;
    unsigned int modificado;
    unsigned int lock;
    uint32_t pos_offload;    // 👈 AGREGAR: Bloque asignado en el archivo Offload
} t_entrada_clock_m;

typedef struct 
{
    uint32_t marco;
    uint32_t timestamp;
    unsigned int presencia;
    unsigned int modificado;
    unsigned int lock;
    uint32_t pos_offload;    // 👈 AGREGAR: Bloque asignado en el archivo Offload
} t_entrada_lru;

typedef struct 
{
    int numero_de_marco;    // Índice del marco (0, 1, ..., marcos_totales-1)
    bool esta_libre;        // true = libre, false = ocupado
    bool esta_lockeado;     // true = lockeado (No elegible como víctima), false = normal
    uint32_t jid_dueno;     // ID del Job
    int nro_pagina_dueno;   // Número de página del Job
} t_marco_control;

typedef struct
{
	uint32_t jid;
	t_list * tabla;
} t_tabla_de_pagina;

extern int tam_memoria;
extern char * path_instrucciones;
extern char * path_offload;
extern int tam_pagina;
extern t_bitarray * bitmap_offload;
extern int cantidad_bloques_totales; 
extern char * algoritmo_reemplazo;
extern int marcos_totales;
extern t_marco_control * vector_de_marcos_memoria;

t_codigo_proceso * crear_codigo_proceso (uint32_t jid, char * path_job);
t_tabla_de_pagina * crear_tabla_paginas_proceso (uint32_t jid);
void destruir_codigo_proceso (void *elemento);
void destruir_tabla_de_pagina (void* elemento);
void imprimir_instrucciones (void *elemento);
void * crear_memoria_de_datos (void);
FILE * crear_offload (int tamano_offload);
void destruir_offload (FILE * ptr_file);
uint32_t asignar_espacio_job(t_control_job* job, uint32_t bytes_a_asignar);
int obtener_bloque_libre_offload (void);
void liberar_bloque_offload(uint32_t nro_bloque);
void* crear_entrada_pagina (uint32_t bloque_offload);
int contar_marcos_libres(void);
int contar_bloques_ocupados_offload (void);
int contar_marcos_lockeados (void);

#endif

