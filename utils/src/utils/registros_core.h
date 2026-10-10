#ifndef _REGISTROS_CORE_H_
#define _REGISTROS_CORE_H_

// Estructura que modela el Contexto de Ejecución del Core 
typedef struct //Se replica esta estructura en modulo Core
{
    uint32_t PC;  // Program Counter
    uint32_t P0;  // Registros de dirección lógica de memoria
    uint32_t P1;
    uint32_t P2;
    uint32_t P3;
    uint32_t P4;
    uint32_t P5;
    uint32_t AX;  // Registros numéricos de propósito general
    uint32_t BX;
    uint32_t CX;
    uint32_t DX;
    uint32_t E1;  // Registros para operandos de entrenamiento
    uint32_t E2;
    uint32_t E3;
} t_registros_core;

#endif
