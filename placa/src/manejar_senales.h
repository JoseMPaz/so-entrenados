#ifndef _MANEJAR_SENALES_H_
#define _MANEJAR_SENALES_H_

#include <signal.h> 
#include <pthread.h>
#include <stdbool.h>

extern pthread_t hilo_consola;
extern pthread_t hilo_servidor;
extern int socket_escucha_global;
extern volatile sig_atomic_t salir;

void manejar_sigint_placa(int sig); 

#endif
