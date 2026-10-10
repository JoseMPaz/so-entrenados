#include "main.h"

/*
Compilar: make
Ejecutar: ./bin/planificador planificador.config no_hacer_nada.asm
*/

int main(int argc, char * argv[]) 
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
	
	t_validar_argumentos validar_argumentos = validar_argumentos_planificador;//Setea la funcion validar_argumentos
	t_cargar_configuracion cargar_configuracion = cargar_configuracion_planificador;//Setea la funcion cargar_configuracion
	t_config * configuracion = NULL;
	t_paquete * paquete;
	t_operacion operacion;
	int socket_escucha;
	pthread_t hilo_servidor;
	
	validar_argumentos (argc, argv);
	configuracion = crear_configuracion (argv[PATH_CONFIG]);
	cargar_configuracion (configuracion);
	historial = crear_historial ( "planificador.log", "PLANIFICADOR_LOG", log_level); 
	
	
	cores = list_create ();// Esta lista permite administrar los cores que se tiene a disposicion para ejecutar los Jobs
	if (sem_init(&mutex_cores, 0, 1/*Protege un recurso lista*/) != 0) //
	{
        perror("Error al inicializar el semáforo mutex_cores");
        return EXIT_FAILURE;
    }
    
    if (sem_init(&sem_core_disponible, 0, 0/*Inicia sin cores disponibles*/) != 0) 
	{
        perror("Error al inicializar el semáforo sem_core_disponible");
        return EXIT_FAILURE;
    }
	        
    /* == Inicio Handshake Planificador - Placa == */
    socket_planificador_to_placa = crear_socket (CLIENTE, ip_placa, puerto_placa);
    conectar_a_servidor (socket_planificador_to_placa, ip_placa, puerto_placa);
    log_info (historial, "%s%s", "Solicitud de conexión: ", "## Módulo: Placa");
    paquete = crear_paquete (SOLICITUD_CONEXION_PLANIF_TO_PLACA);
    enviar_paquete (paquete, socket_planificador_to_placa);
	destruir_paquete (paquete);
	operacion = recibir_operacion (socket_planificador_to_placa);
	
	if (operacion == RESPUESTA_CONEXION_PLACA_TO_PLANIF_OK)
	{
		log_info (historial, "%s%s", "Conexión confirmada: ", "## Módulo: Placa");
	}
	else
	{
		fprintf (stderr, "%s\n", "Error: No se pudo conectar Planificador a Placa");
		exit(1);
	}
    /* == Final Handshake Planificador - Placa == */
    
    
    /* == Inicio Handshake Planificador - Storage == */
		//Aca hara lo mismo pero para storage - Por ahora no me voy a conectar aun a storage
	/* == Final Handshake Planificador - Storage == */
	
	/*Servidor para aceptar conexiones de core*/
	socket_escucha = crear_socket (SERVIDOR, NULL, puerto_escucha);
	pthread_create (&hilo_servidor, NULL, servidor_planificador, (void *) &socket_escucha);
	
	/*Planificador de corto plazo*/
	
	/*Planificador de corto plazo*/
	
	/*Crear JBC del proceso padre y se encola en new, luego el planificador de largo plazo al detectarlo
	y si el grado de multiprogramacion se lo permite, pide a placa que cree su estructura administrativa,
	le envia el jid y path_job, una vez placa le confirma que lo pudo hacer, se transiciona 
	el job desde new a ready */
	
	
    pthread_join (hilo_servidor, NULL);
    
    destruir_historial (historial);
	destruir_configuracion (configuracion);
	close(socket_planificador_to_placa);
	sem_destroy(&mutex_cores);
	destruir_lista_cores (cores);
    saludar("planificador");
    return 0;
}

/*
Módulo: Planificador

Administra la cola de Jobs y gestiona la planificación de los Jobs.

> ./bin/planificador [Archivo Config] [Path Job Inicial]

El primer parámetro que será el archivo de configuración. 
El segundo parámetro será el Path al Job Inicial, este Job tendra identificador JID = 0, 
a partir del cual se crearán los demás Jobs.
-------------------------------------------------------------------------------------------------
Al iniciar se conectara con el modulo Placa y con el modulo Storage, 
Luego creara un servidor multihilo para atender de forma concurrente las peticiones de los Cores.

A lo largo de la ejecución se podrán desconectar y conectar nuevos Cores, 
por lo que se deberá mantener la escucha a nuevas conexiones siempre activa.
-------------------------------------------------------------------------------------------------
Planificación de Jobs:

Los planificadores de largo y corto plazo deberá ser capaz de gestionar un modelo de 5 estados:
New, Ready, Exec, Block, Exit.
-------------------------------------------------------------------------------------------------
Contexto de Ejecución:

Por cada Job del sistema se deberá almacenar un contexto de ejecución que consiste, como mínimo,
en una copia de todos los registros del Core inicializados en 0 al momento de crear el Job. 

El contexto:

Al poner a ejecutar un Job en un Core, se le debe enviar el contexto de ejecución, y el Core lo devolverá al planificador actualizado al liberarlo.
-------------------------------------------------------------------------------------------------
Planificación de Largo Plazo:

*Se encargará de ingresar Jobs del estado NEW al estado READY.
En caso de que el grado máximo de multiprogramación (definido por archivo de configuración) lo
permita, los Jobs pasarán de New a READY mediante el algoritmo FIFO. Dado que los Jobs inician sin
memoria asignada, este pasaje no requiere ninguna verificación sobre el espacio disponible en la
Placa.

*Cuando un Job finalice, se debe pasar a EXIT, liberando las estructuras asociadas al Job en todo el sistema.
-------------------------------------------------------------------------------------------------
Planificación de Corto Plazo:

Los algoritmos definidos en el archivo de configuración son: FIFO, RR y HRRN. 

Para HRRN se tendrá un valor estimado inicial definido por archivo de configuración.
-------------------------------------------------------------------------------------------------
Page Fault:

Si un Core informa al planificador un Fallo de Pagina en la pagina x
*Se debe bloquear al Job 
*Solicitar al modulo Placa la carga de la página x. 
*Una vez cargada la pagina, el Job deberá pasar de Block a READY.
-------------------------------------------------------------------------------------------------
Atención de Syscalls:

Un Core puede pedir al planificador atender una Syscall Bloqueante o No Bloqueante.

Si la Syscall es bloqueante: El Core devuelve al Planificador el JID y el contexto de ejecución actualizado. 
El Planificador guarda este contexto de ejecucion en el JBC del Job, 
Al haberse liberado el Core, deberá invocar al planificador de corto plazo para asignarle un nuevo Job de la cola de READY.

Si la Syscall es No bloqueante: No bloqueen al Job y no liberarán el Core. 
Una vez finalizadas la syscall se le deberá responder al Core que realizó la llamada para que continúe con su ejecución.

Las Syscalls que operan sobre la memoria del Job son:
LOAD_BATCH
LABEL
REPORT
SAVE_CHECKPOINT
LOAD_CHECKPOINT 

Estas syscall de memoria llegan con las DF ya traducidas por el Core ( por cada página involucrada -> un par [DF;Tamaño] ) 
y con sus páginas lockeadas en la Placa (page locking: consiste en marcar una página o marco temporalmente en la placa 
para que no pueda ser removida de la memoria por un reemplazo de página.). 
Una vez finalizada la syscall, se deberá solicitar a la Placa el deslockeo de las páginas del Job antes de pasarlo al estado READY.

Ante la desconexión de un Core del planificador: El Job que estuviera ejecutando en el Core, deberá volver al estado READY
y el planificador debe solicitar a la Placa el deslockeo de sus páginas.
-------------------------------------------------------------------------------------------------
Las Syscalls de Jobs son:
INIT_JOB
EXIT

INIT_JOB: Recibe el nombre de un archivo de pseudocódigo y deberá crear un nuevo JBC en el estado NEW, 
sin bloquear al JBC que solicitó la syscall.
EXIT: Debe finalizar el JBC que solicitó la syscall, de acuerdo a lo definido en la Planificación de Largo Plazo.
-------------------------------------------------------------------------------------------------
Las syscall para pedir Memoria de usuario son:
ALLOC
FREE

ALLOC y FREE se deberán derivar a la Placa y no bloquearán al Job. 
En el caso de ALLOC, el planificador deberá responder al Core la DL informada por la Placa.
-------------------------------------------------------------------------------------------------
Las syscall para pedir Servicios son:
LOAD_BATCH
LABEL
REPORT 

Se deberán atender de acuerdo a lo definido en la sección de Servicios, bloqueando al Job hasta finalizar la operación.
-------------------------------------------------------------------------------------------------
Las syscall para pedir Checkpoints son:
SAVE_CHECKPOINT
LOAD_CHECKPOINT
DELETE_CHECKPOINT

Las Syscalls de Checkpoints bloquearán al Job hasta finalizar la operación.
Para la syscall SAVE_CHECKPOINT, el Planificador recibirá del Core el nombre de un Checkpoint, un
tamaño y las DF correspondientes a ese rango. 
Se deberán leer de la memoria del Job dicha cantidad de bytes y enviarlos al Storage para que los persista con dicho nombre.

Para la syscall LOAD_CHECKPOINT, el Planificador recibirá del Core el nombre de un Checkpoint, 
un tamaño y las DF correspondientes a ese rango. 
Se deberá solicitar su contenido al Storage y escribir dicha cantidad de bytes en la memoria del Job.

Para la syscall DELETE_CHECKPOINT, el Planificador recibirá del Core el nombre de un Checkpoint y
deberá solicitar su eliminación al Storage.
-------------------------------------------------------------------------------------------------
Servicios del planificador:
LOAD_BATCH
LABEL
REPORT

El Planificador deberá implementar 3 Servicios propios: Loader, Labeler y Logger. Cada uno atenderá
una única petición por vez, bajo el algoritmo FIFO. En todos los casos se deberá bloquear al Job que
solicitó la operación y mantener una cola de espera por cada Servicio.

Para el Loader, syscall LOAD_BATCH, el Planificador recibirá del Core el nombre de un archivo, 
un tamaño y las DF correspondientes a ese rango. Se deberán leer dicha cantidad de bytes de ese archivo, 
ubicado en el directorio definido por archivo de configuración, y escribirlos en
la memoria del Job. Esta operación tendrá un retardo definido por archivo de configuración.

Para el Labeler, syscall LABEL, el Planificador recibirá del Core un tamaño y las DF correspondientes a ese rango. 
Se le deberá pedir al usuario el ingreso de dicha cantidad de caracteres por teclado y, 
una vez recibidos, escribirlos en la memoria del Job.

Para el Logger, syscall REPORT, el Planificador recibirá del Core un tamaño y las DF
correspondientes a ese rango. Se deberán leer de la memoria del Job dicha cantidad de bytes y
escribirlos en formato ASCII Hexadecimal en el archivo de reportes definido por archivo de
configuración, que deberá ser distinto del archivo de log del módulo.
------------------------------------------------------------------------------------------------- 
Estadísticas
Sobre cada Job ejecutado, se deberán calcular las siguientes estadísticas:

Tiempo total de ejecución (desde el inicio a su finalización)
Tiempo acumulado de espera (considerando solo el estado READY)
Tiempo acumulado para ingresar (considerando solo el estado NEW)
Tamaño máximo de memoria dinámica ocupada en un momento dado
Cantidad acumulada de syscalls solicitadas
Cantidad acumulada de page faults generados
Cantidad acumulada de transiciones READY => EXEC

Las mismas deberán ser impresas en el Log cuando el mismo finalice.












*/
