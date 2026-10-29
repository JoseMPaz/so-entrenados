#ifndef _REGISTROS_CORE_H_
#define _REGISTROS_CORE_H_

// Estructura que modela el Contexto de Ejecución del Core 
typedef struct //Se replica esta estructura en modulo Core
{
    uint32_t pc;  // Program Counter
    uint32_t p0;  // Registros de dirección lógica de memoria
    uint32_t p1;
    uint32_t p2;
    uint32_t p3;
    uint32_t p4;
    uint32_t p5;
    uint32_t ax;  // Registros numéricos de propósito general
    uint32_t bx;
    uint32_t cx;
    uint32_t dx;
    uint32_t e1;  // Registros para operandos de entrenamiento
    uint32_t e2;
    uint32_t e3;
} t_registros_core;

#endif
