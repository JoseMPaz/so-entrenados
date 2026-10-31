#include "manejar_senales.h"

void manejar_sigint_placa(int sig)
{
    bool valor_salir = true;
    __atomic_store_n(&salir, valor_salir, __ATOMIC_SEQ_CST);
    
    if (socket_escucha != -1) 
    {
        shutdown(socket_escucha, SHUT_RDWR); 
        close(socket_escucha); 
    }
    
    // 🔥 FORZAMOS LA CANCELACIÓN: Rompe cualquier bloqueo de fgets, getline o accept al instante
    pthread_cancel(hilo_consola);
    pthread_cancel(hilo_servidor);
}
