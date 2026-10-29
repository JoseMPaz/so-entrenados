#ifndef _MANEJAR_SENALES_H_
#define _MANEJAR_SENALES_H_

#include <signal.h> 
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h> // 👈 CLAVE: Para que conozca sem_t
#include <unistd.h>    // 👈 CLAVE: Para que conozca close()
#include <sys/socket.h> // 👈 CLAVE: Agregá esta librería para conocer SHUT_RDWR

extern pthread_t hilo_servidor;
extern pthread_t hilo_ingreso_de_jobs;
extern pthread_t hilo_egreso_de_jobs;
extern int socket_escucha_global;
extern volatile sig_atomic_t salir;

// 👈 AGREGÁ TUS SEMÁFOROS ACÁ COMO EXTERN:
extern sem_t sem_ingresar_job_a_new;
extern sem_t sem_ingresar_job_a_ready;
extern sem_t sem_ingresar_job_a_exit;
extern sem_t sem_grado_multiprogramacion;

void manejar_sigint_planificador(int sig); 

#endif

