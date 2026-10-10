#ifndef _OPERACIONES_H_
#define _OPERACIONES_H_

typedef enum
{
	// ==========================================
    // 1. PETICIONES HACIA LA PLACA
    // ==========================================
    // Desde el Core:
    CORE_TO_PLACA_FETCH_INSTRUCTION,
    CORE_TO_PLACA_GET_FRAME,
    CORE_TO_PLACA_READ_DATA,
    CORE_TO_PLACA_WRITE_DATA,
    CORE_TO_PLACA_UNLOCK_PAGES,

    // Desde el Planificador:
    PLANIF_TO_PLACA_CREATE_JOB,
    PLANIF_TO_PLACA_ALLOC_SPACE,
    PLANIF_TO_PLACA_FREE_SPACE,
    PLANIF_TO_PLACA_LOAD_PAGE,
    PLANIF_TO_PLACA_READ_DATA,
    PLANIF_TO_PLACA_WRITE_DATA,
    PLANIF_TO_PLACA_UNLOCK_PAGES,
    PLANIF_TO_PLACA_EXIT_JOB,

    // ==========================================
    // 2. COMUNICACIÓN CORE <-> PLANIFICADOR
    // ==========================================
    CORE_TO_PLANIF_PAGE_FAULT,
    CORE_TO_PLANIF_INTERRUPT,
    CORE_TO_PLANIF_SYSCALL_INIT_JOB,
    CORE_TO_PLANIF_SYSCALL_ALLOC,
    CORE_TO_PLANIF_SYSCALL_FREE,
    CORE_TO_PLANIF_SYSCALL_LOAD_BATCH,
    CORE_TO_PLANIF_SYSCALL_LABEL,
    CORE_TO_PLANIF_SYSCALL_REPORT,
    CORE_TO_PLANIF_SYSCALL_SAVE_CHECKPOINT,
    CORE_TO_PLANIF_SYSCALL_LOAD_CHECKPOINT,
    CORE_TO_PLANIF_SYSCALL_DELETE_CHECKPOINT,
    CORE_TO_PLANIF_SYSCALL_EXIT,
    
    // Respuestas del Planificador al Core para Syscalls no bloqueantes (como ALLOC):
    PLANIF_TO_CORE_ALLOC_OK,

    // ==========================================
    // 3. PETICIONES DE PLANIFICADOR A STORAGE
    // ==========================================
    PLANIF_TO_STORAGE_SAVE_CHECKPOINT,
    PLANIF_TO_STORAGE_LOAD_CHECKPOINT,
    PLANIF_TO_STORAGE_DELETE_CHECKPOINT,
    
    // ==========================================
    // 4. CONEXIONES DE MODULOS
    // ==========================================
    SOLICITUD_CONEXION_PLANIF_TO_PLACA,
    RESPUESTA_CONEXION_PLACA_TO_PLANIF_OK,
    SOLICITUD_CONEXION_CORE_TO_PLACA,
    RESPUESTA_CONEXION_PLACA_TO_CORE_OK,
    SOLICITUD_CONEXION_PLANIF_TO_STORAGE,
    RESPUESTA_CONEXION_STORAGE_TO_PLANIF_OK,
    SOLICITUD_CONEXION_CORE_TO_PLANIF,         
    RESPUESTA_CONEXION_PLANIF_TO_CORE_OK,
    RESPUESTA_CONEXION_FALLIDA,

    // ==========================================
    // 5. RESPUESTAS GENÉRICAS PARA OPERACIONES
    // ==========================================
    OPERACION_OK,
    OPERACION_ERROR
} t_operacion;

#endif
