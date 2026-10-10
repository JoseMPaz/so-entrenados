#include "main.h"

/*
Compilar: make
Ejecutar: ./bin/placa placa.config
*/

/* Teoria
Cantidad de Marcos(memoria_de_datos) = tam_memoria / tam_pagina
un Job puede tener muchas más páginas que "Cantidad de Marcos(memoria_de_datos)"
*/

int main (int argc, char * argv[]) 
{
	struct sigaction accion = {0};// Inicializa la estructura 'accion' llenando todos sus campos con 0.

	// Asigna la función 'manejar_sigint' como el manejador (handler) de la señal.
	// Esta es la función que se ejecutará automáticamente cuando llegue la señal.
	accion.sa_handler = manejar_sigint;
	// Inicializa y vacía el conjunto de señales 'sa_mask'.
	// Durante la ejecución del handler, no se bloqueará ninguna otra señal adicional.
	sigemptyset(&accion.sa_mask);
	// Configura las flags en 0. Esto desactiva comportamientos especiales y usa
	// el comportamiento predeterminado del sistema operativo para esta señal.
	accion.sa_flags = 0;
	
	// Registra la nueva configuración asociando la señal SIGINT (Ctrl+C) con 'accion'.
	// Si la llamada al sistema devuelve -1, significa que hubo un error en el registro.
	if (sigaction(SIGINT, &accion, NULL) == -1)
	{
    	perror("sigaction");// Imprime en la salida de error estándar (stderr) el motivo del fallo
    	return EXIT_FAILURE; // Finaliza el programa indicando un estado de error al sistema operativo
	}
	
	t_validar_argumentos validar_argumentos = validar_argumentos_placa;//Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_placa;//Setea la funcion cargar_configuracion
	t_config * configuracion = NULL;
	int socket_escucha;
	pthread_t hilo_consola;
	pthread_t hilo_servidor;
	
	validar_argumentos (argc, argv);//Valida los Argumentos ingresados por la Linea de Comando al ejecutar el modulo
	configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	historial = crear_historial ( "placa.log", "PLACA_LOG", log_level); 
	
	log_info (historial, "Inicio del Módulo Placa");// LOG 1: Optativo
	
	instrucciones_de_jobs = list_create ();	/*Estructura administrativa, sus nodos tienen pid e instrucciones*/
	memoria_de_datos = crear_memoria_de_datos ();/*Espacio  de trabajo de los Jobs (void *) */
	offload = crear_offload ();
	
	/*Inicio Prueba de memoria de instrucciones*/
	list_add (instrucciones_de_jobs, crear_codigo_proceso (7, "no_hacer_nada.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (15, "setear_registros_proposito_general.asm"));
	list_add (instrucciones_de_jobs, crear_codigo_proceso (3, "suma_resta.asm"));
	
	list_iterate(instrucciones_de_jobs, imprimir_instrucciones);
	/*Fin Prueba de memoria de instrucciones*/
	
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);	
	
	pthread_create (&hilo_servidor, NULL, servidor_placa, (void *) &socket_escucha);
	pthread_create (&hilo_consola, NULL, consola, NULL);
	
	pthread_join (hilo_servidor, NULL);
	pthread_join (hilo_consola, NULL);
	
	/*Acá no deberia llegar ya que servidor_placa nunca finaliza su ejecución*/
	/*Destruir*/
	list_destroy_and_destroy_elements (instrucciones_de_jobs, destruir_codigo_proceso);
	free (memoria_de_datos);
	
	destruir_historial (historial);
	destruir_configuracion (configuracion);
	destruir_offload (offload);
		
    saludar("placa");
    
    return 0;
}

/*
Módulo: Placa

✅Este módulo se encarga de administrar la memoria del sistema. 
✅En él se almacenan:
✅Las instrucciones de los Jobs 
✅El espacio de trabajo sobre el que operan los Jobs: memoria_de_datos.
------------------------------------------------------------------------------------------------- 
✅Lineamiento e Implementación
------------------------------------------------------------------------------------------------- 
✅Al iniciar el modulo Placa, se crea un servidor multihilo que se encargará de gestionar de manera concurrente las
✅peticiones de conexion del Planificador y de los Cores y tambien para que haga algo por ellos. 

✅A lo largo de la ejecución se podrán conectar nuevos Cores, por lo que se deberá mantener la escucha a 
✅nuevas conexiones siempre activa.
✅> ./bin/placa placa.config
------------------------------------------------------------------------------------------------- 
✅Memoria de Instrucciones
------------------------------------------------------------------------------------------------- 
✅En esta sección la Placa contendrá las instrucciones de los Jobs, 
✅que le serán solicitadas por los diferentes Cores. 

✅Dicho espacio será ilimitado en capacidad (a diferencia de la memoria de usuario)

✅Los archivos de pseudocódigo serán archivos de texto que contendrán dichas instrucciones, separadas por el caracter ‘\n’. 

✅Por cada Job del sistema se tendrá un archivo de instrucciones, 
✅para lo que cada grupo deberá implementar una estructura que le permita asociar qué Job tiene asociado qué archivo.

✅La Placa deberá ser capaz de enviar la instrucción correspondiente a cada pedido del Core 
✅sin haber una restricción específica de cómo se deben obtener las instrucciones.
------------------------------------------------------------------------------------------------- 
✅Memoria de Usuario
------------------------------------------------------------------------------------------------- 
✅Esquema de memoria y Estructuras:
La memoria de datos de la Placa utilizará un esquema de paginación con memoria virtual, 
con asignación dinámica y alcance global. 

✅El tamaño total de 
✅*la memoria de usuario y 
✅*el tamaño de página 
✅estarán definidos por archivo de configuración.

✅Se deberá implementar paginación bajo demanda: 
✅los Jobs empezarán sin frames asignados, 
✅y una página pasará a ocupar un marco recién cuando el Job la acceda.
------------------------------------------------------------------------------------------------- 
La Placa contará como mínimo con 2 estructuras:

✅●​ Un espacio contiguo de memoria (representado por un void *), que será la memoria de datos del sistema.
✅●​ Una tabla de páginas por cada Job.

✅Las tablas de páginas: son estructuras administrativas que no deben guardarse dentro de la memoria de datos.

El Core deberá solicitarle a la Placa:
el nro_marco correspondiente a cada nro_página a la que necesite acceder.
------------------------------------------------------------------------------------------------- 
El Offload
Es un archivo administrado por la Placa, cuyo:
*path y 
*tamaño 
se definen por archivo de configuración. 

En él se almacenan las páginas de los Jobs que no se encuentran en la memoria de datos de la Placa.

Al imitar los accesos a disco, el Offload deberá implementarse como si fuera un único hilo de ejecución: 
no se podrán realizar dos o más lecturas o escrituras en paralelo.
------------------------------------------------------------------------------------------------- 
Reemplazo de páginas
Cuando se deba cargar una página en la memoria de datos de la Placa y no existan marcos libres, 
se deberá seleccionar una página víctima entre todas las páginas del conjunto residente(Conjunto de páginas de un Job, actualmente cargadas en la Placa) que no se encuentren lockeadas, sin importar a qué Job pertenezcan. 

El algoritmo de reemplazo se definirá por archivo de configuración, pudiendo ser:
*LRU 
*Clock Modificado.
------------------------------------------------------------------------------------------------- 
Operaciones
✅La Placa, al ser el módulo que gestiona la memoria del sistema y de los Jobs, puede aceptar una serie de operaciones, tales ✅como:
------------------------------------------------------------------------------------------------- 
Creación de Job
Para esta operación se va a recibir un JID y el nombre de un archivo de pseudocódigo, relativo al 
path base configurado por archivo de configuración. 
Con los datos recibidos, la Placa deberá crear las estructuras administrativas necesarias para el Job.
------------------------------------------------------------------------------------------------- 
Asignación de espacio
Al momento de asignar espacio la Placa recibirá como parámetro:

*el JID del Job y 
*el tamaño en bytes a asignar, 

y deberá responder la dirección lógica en la que comienza el espacio asignado. 
Cada asignación deberá comenzar al inicio de una página.
Se deberá reservar en el Offload el lugar necesario para respaldar las páginas asignadas. 
En caso de no contar con espacio suficiente, la operación no se podrá realizar y se le deberá informar al
Planificador para que finalice el Job.
------------------------------------------------------------------------------------------------- 
Liberación de espacio
Al momento de liberar espacio se recibirá el JID del Job y la dirección lógica en la que comienza un
espacio previamente asignado. 
Se deberán liberar los marcos ocupados por sus páginas y el lugar que estas tenían reservado en el Offload.
------------------------------------------------------------------------------------------------- 
Obtención de marco
Esta operación es solicitada por el Core al momento de realizar la traducción de la MMU. 
Recibirá como parámetro el JID y el número de página. 
La Placa deberá consultar la Tabla de Páginas del Job:

●​ Si la página se encuentra residente en memoria, le responderá al Core con 
el número de marco correspondiente. La página pasará a estar lockeada.

●​ Si la página no se encuentra residente, le responderá al Core indicando que ocurrió un Page Fault.
------------------------------------------------------------------------------------------------- 
Carga de página
Esta operación será solicitada por el Planificador cuando un Core informe un Page Fault. 
Se recibirá el JID del Job y un número de página, y se deberá cargar la misma en la memoria de la Placa.
------------------------------------------------------------------------------------------------- 
Lectura de datos
Esta operación puede llegar de un Core o del Planificador, y en ambos casos se recibirá 

el JID, 
una dirección física y 
un tamaño a leer. 

Se deberá responder el contenido del rango pedido.
------------------------------------------------------------------------------------------------- 
Escritura de datos
Esta operación recibirá los mismos parámetros que la de lectura, junto con el contenido a escribir.
------------------------------------------------------------------------------------------------- 
Deslockeo de páginas
Esta operación será enviada por el Core cada vez que finalice con éxito un ciclo de instrucción, 
o cuando devuelva el contexto al Planificador (por ejemplo, al sufrir un Page Fault). 

En el caso de las Syscalls que operan sobre la memoria del Job, será enviada por el Planificador al finalizar la operación.

La Placa recibirá el JID y deberá liberar el estado de lockeo de todas sus páginas, 
permitiendo que los marcos asignados vuelvan a ser elegibles como víctimas de reemplazo.
------------------------------------------------------------------------------------------------- 
Finalización de Job
Esta operación se utiliza para finalizar un Job, por lo que solamente va a recibir un JID. 
A partir del mismo se deberán liberar 
todos los marcos que tuviera ocupados, 
el lugar que tuviera reservado en el Offload y 
todas las estructuras asociadas al mismo.
------------------------------------------------------------------------------------------------- 
Comandos locales
La Placa deberá implementar una consola por stdin, soportando los siguientes comandos. Estos
responderán mostrando información en un formato “human-readable”:

INFO: mostrar 
*porcentaje de memoria/offload ocupada, 
*cantidad de frames libres/lockeados/totales, 
*cantidad de Jobs actuales

TLS: listar 
*todos los Jobs registrados, indicando para cada uno: 
	ID, 
	cantidad de páginas del conjunto residente y 
	cantidad de páginas totales.
Se podrán agregar otros comandos que se consideren necesarios, siempre y cuando no modifiquen
ni contradigan el funcionamiento detallado en el módulo.
------------------------------------------------------------------------------------------------- 
Control Obligatorio 1: Conexiones y Serialización
*la Placa inicializa su servidor multihilo.
*recibir del core un handshake inicial para identificarse y recibir un paquete de mensajes serializado  y 
enviar como prueba otro paquete de mensajes serializado.
------------------------------------------------------------------------------------------------- 
Control Obligatorio 2: Planificación y Ejecución Básica
Nada
------------------------------------------------------------------------------------------------- 
Control Obligatorio 3: Memoria, Paginación y Servicios, Storage
En la Placa, implementar:
*tablas de páginas, 
*paginación bajo demanda, 
*algoritmo de reemplazo y Offload(Swap).

------------------------------------------------------------------------------------------------- 
Control Obligatorio 4: Storage, Filesystem y Persistencia



*/
