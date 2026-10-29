#ifndef _CONEXION_H_
#define _CONEXION_H_

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <string.h>
#include <signal.h> 
#include <pthread.h>
#include <commons/collections/list.h>
#include "operaciones.h"

typedef enum
{
	CLIENTE = 0,
	SERVIDOR = 1
}t_conexion;

typedef struct
{
	int longitud;
	char * flujo;
}t_carga_util;

typedef struct
{
	t_operacion operacion;
	t_carga_util * carga_util;
}t_paquete;

extern volatile sig_atomic_t salir;


int crear_socket (t_conexion tipo_conexion, const char * ip, const char * puerto);
void conectar_a_servidor (int socket_cliente, char * ip_servidor, char * puerto_servidor);
int recibir_operacion (int socket);
void enviar_paquete (t_paquete * paquete, int socket);
void * serializar_paquete (t_paquete * paquete, int bytes_a_enviar);
void agregar_a_paquete (t_paquete * paquete, void * cadena, int longitud);
t_paquete * crear_paquete (t_operacion operacion);
void destruir_paquete (t_paquete * paquete);
t_list * recibir_carga_util (int socket);
void manejar_sigint(int sig); 


#endif
