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

typedef enum
{
	CLIENTE = 0,
	SERVIDOR = 1
}t_conexion;

int crear_socket (t_conexion tipo_conexion, const char * ip, const char * puerto);
void conectar_a_servidor (int socket_cliente, char * ip_servidor, char * puerto_servidor);

#endif
