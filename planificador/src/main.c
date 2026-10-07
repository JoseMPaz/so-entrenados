#include <utils/hello.h>
#include <utils/cla.h>

int main(int argc, char * argv[]) 
{
	t_validar_argumentos validar_argumentos = validar_dos_argumento;
	validar_argumentos (argc, argv);
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
