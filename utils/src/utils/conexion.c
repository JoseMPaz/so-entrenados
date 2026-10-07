#include "conexion.h"


int crear_socket (t_conexion tipo_conexion, const char * ip, const char * puerto)
{
	int conexion;
	int err;

	struct addrinfo hints;
	struct addrinfo * server_info;
	
	memset ( &hints , 0 , sizeof(hints) );
	hints.ai_family 	= AF_INET;
	hints.ai_socktype 	= SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
		
	if (tipo_conexion == CLIENTE)
	{
		err = getaddrinfo ( ip , puerto , &hints , &server_info );//Pide memoria para server_info
		conexion = socket ( server_info->ai_family, server_info->ai_socktype , server_info->ai_protocol);
		freeaddrinfo(server_info);//devuelve memoria de server_info
	}
	if (tipo_conexion == SERVIDOR)
	{	
		err = getaddrinfo ( NULL , puerto , &hints , &server_info );//Pide memoria para server_info
		conexion = socket ( server_info->ai_family, server_info->ai_socktype , server_info->ai_protocol);
		
		err = setsockopt ( conexion , SOL_SOCKET , SO_REUSEPORT , &(int){1} , sizeof(int) );
		err = bind ( conexion , server_info->ai_addr , server_info->ai_addrlen );
		err = listen ( conexion , SOMAXCONN );
		if (err == -1) 
		{
    		puts("No se pudo hacer listen");
    		abort();
		}
		freeaddrinfo(server_info);//devuelve memoria de server_info
	}
		
	return conexion;
}

void conectar_a_servidor (int socket_cliente, char * ip_servidor, char * puerto_servidor)
{	
	struct addrinfo hints;
	struct addrinfo * server_info;
	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	getaddrinfo(ip_servidor, puerto_servidor, &hints, &server_info);//Pide memoria para server_info
	
	if ( connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen) == -1 )
	{
		fprintf (stderr, "%s\n", "No se logro establecer la conexion hacia el servidor");
		fprintf (stderr, "IP: %s, PUERTO: %s\n", ip_servidor, puerto_servidor);
		freeaddrinfo(server_info);//devuelve memoria de server_info
		exit (EXIT_FAILURE);
	}
	freeaddrinfo(server_info);//devuelve memoria de server_info
	return;
}

