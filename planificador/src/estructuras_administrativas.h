#ifndef _ESTRUCTURAS_ADMINISTRATIVAS_H_
#define _ESTRUCTURAS_ADMINISTRATIVAS_H_

#include <stdint.h>
#include <stdlib.h>
#include <commons/collections/list.h>
#include <commons/string.h>
#include <stdio.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdint.h>
#include <commons/temporal.h> // Para manejar los tiempos de las estadísticas
#include <utils/registros_core.h> //Lo manipula planificador y core

/* == Para administrar a los Core == */
typedef enum 
{
    CORE_LIBRE,
    CORE_OCUPADO
} t_estado_core;

typedef struct 
{
    int id_core;
    int socket;
    t_estado_core estado;
} t_core;

/* == Para administrar a los Jobs == */

typedef enum /* Modelo de 5 estados */
{
    ESTADO_NEW,
    ESTADO_READY,
    ESTADO_EXEC,
    ESTADO_BLOCK,
    ESTADO_EXIT
} t_estado_job;

typedef struct 
{
    int64_t tiempo_total_ejecucion;          // Desde el inicio a su finalización
    int64_t tiempo_acumulado_espera_ready;   // Solo en estado READY
    int64_t tiempo_acumulado_ingresar_new;   // Solo en estado NEW
    uint32_t max_memoria_dinamica_ocupada;   // Tamaño máximo alcanzado con ALLOC
    uint32_t cantidad_syscalls;              // Solicitadas acumuladas
    uint32_t cantidad_page_faults;           // Generados acumulados
    uint32_t cantidad_transiciones_ready_exec; // Veces que pasó de READY a EXEC
} t_estadisticas_job;

typedef struct //Job Control Block
{
    uint32_t jid;                          // Identificador numérico unívoco (Job ID) [pág. 9]
    t_estado_job estado;                   // Estado actual del Job
    t_registros_core contexto_de_ejecucion;           //El tipo de datos se define en registros_core.h
    t_estadisticas_job estadisticas;       // Métricas requeridas para el log de EXIT [pág. 12]
    /* Campos Administrativos de Planificación */
    int64_t tiempo_ultima_transicion;      // Timestamp (ms) para calcular cuánto tiempo pasa en un estado
    double proxima_estimacion_rafaga;      // Requerido si el archivo .config activa HRRN, inicia en rafaga estimada
    int64_t tiempo_ingreso_a_ready;          // Timestamp exacto al entrar a READY (útil para FIFO y HRRN)
} t_jcb; 

// Estructuras globales sincronizadas
extern t_list * cores;
extern sem_t mutex_cores;

void destruir_nodo_core(void * elemento);
void destruir_lista_cores (t_list * lista_a_destruir);

#endif

