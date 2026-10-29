#include "manejar_senales.h"

void manejar_sigint_planificador (int sig)
{
    bool valor_salir = true;
    __atomic_store_n(&salir, valor_salir, __ATOMIC_SEQ_CST);
    
    // 🔥 SOLUCIÓN DEFINITIVA DE RED:
    // shutdown() destruye los canales de lectura y escritura del socket.
    // Esto obliga a accept() a destrabarse de inmediato devolviendo -1.
    if (socket_escucha != -1) 
    {
        shutdown(socket_escucha, SHUT_RDWR); // 👈 FORZAMOS EL DESBLOQUEO AQUÍ
        close(socket_escucha); 
    }
    
    // Despertamos el resto de los hilos de planificación
    sem_post(&sem_ingresar_job_a_new);
    sem_post(&sem_ingresar_job_a_ready);
    sem_post(&sem_ingresar_job_a_exit);
    sem_post(&sem_grado_multiprogramacion);
}

