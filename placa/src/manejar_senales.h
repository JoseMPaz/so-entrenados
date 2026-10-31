#ifndef _MANEJAR_SENALES_H_
#define _MANEJAR_SENALES_H_

#include <signal.h> 
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h> // 👈 CLAVE: Para que conozca sem_t
#include <unistd.h>    // 👈 CLAVE: Para que conozca close()
#include <sys/socket.h> // 👈 CLAVE: Agregá esta librería para conocer SHUT_RDWR

extern pthread_t hilo_consola;
extern pthread_t hilo_servidor;
extern int socket_escucha;
extern volatile sig_atomic_t salir;

void manejar_sigint_placa(int sig); 

#endif
