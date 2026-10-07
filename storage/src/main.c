#include <utils/cla.h>
#include <utils/conexion.h>
#include "gestion.h"
#include <utils/configuracion.h>
#define PATH_CONFIG 1

int main(int argc, char* argv[]) 
{
	//int * socket_escucha = NULL;
	//pthread_t hilo_servidor;
	t_validar_argumentos validar_argumentos = validar_un_argumento;
	
	
	validar_argumentos (argc, argv);
	//t_config * config = iniciar_config (argv[PATH_CONFIG]);

	//socket_escucha = (int *) malloc (sizeof(int));
	//*socket_escucha = crear_socket ( SERVIDOR, NULL, config_get_string_value(config,"PUERTO_ESCUCHA") );
    //pthread_create (&hilo_servidor, NULL, procesar_peticiones_storage, (void *)socket_escucha);
    
    saludar("storage");
    
    //pthread_join (hilo_servidor, NULL);
    
    return 0;
}

/*
Módulo: Storage
El Storage es el módulo encargado de persistir los Checkpoints de los Jobs. 
Los mismos sobreviven a la finalización del Job que los generó, por lo que un Job puede cargar un Checkpoint creado por otro.
------------------------------------------------------------------------------------------------- 
Lineamiento e Implementación
Al iniciar creará un servidor multihilo que deberá ser capaz de atender las peticiones del Planificador de manera concurrente.

> ./bin/storage [Archivo Config]

Toda la información que administra el Storage deberá persistirse dentro de la carpeta definida por archivo de configuración, 
de manera que su contenido sobreviva entre ejecuciones del módulo.
------------------------------------------------------------------------------------------------- 
Implementación
El Storage implementará:
*un sistema de archivos cuya especificación será una variante de “FAT32”, la cual daremos en llamar “FAT32_TRAIN”. 
*Junto a él se encontrará el journal, que se detalla en la sección Journaling.
------------------------------------------------------------------------------------------------- 
Estructura interna del sistema de archivos
El volumen de dicho sistema se persistirá en un archivo real del sistema operativo anfitrión (Linux en
nuestro caso). El mismo se dividirá en bloques de igual tamaño, numerados a partir del 0. La cantidad
de bloques y el tamaño de cada uno se definen por archivo de configuración.

Sus bloques se reparten entre cuatro regiones:

●​ El superbloque, que ocupa el bloque 0.

●​ La tabla FAT, cuyo tamaño depende de la cantidad de bloques y del tamaño del puntero.

●​ Un único directorio raíz, que ocupa la cantidad de bloques definida por archivo de configuración.

●​ Los bloques de datos, que son todos los restantes.
Si al iniciar el módulo el archivo del filesystem no existe, deberá ser creado y “formateado”: deberá
escribir el superbloque, la tabla FAT con la totalidad de sus bloques de datos libres y el directorio con
todas sus entradas libres.
------------------------------------------------------------------------------------------------- 
Superbloque
Ocupa el bloque 0 y describe el formato del filesystem. Deberá contar exactamente con la siguiente estructura de datos contigua:

●​ Firma (Magic Number): 11 bytes (Cadena de caracteres estática, "FAT32_TRAIN", para identificar que es un filesystem válido).

●​ Tamaño de bloque: 4 bytes (uint32_t).

●​ Cantidad de bloques del filesystem: 4 bytes (uint32_t).

●​ Cantidad de bloques del directorio: 4 bytes (uint32_t).

El resto del bloque 0 (hasta completar el TAM_BLOQUE definido) deberá llenarse con padding (ceros) y queda reservado para uso futuro.
------------------------------------------------------------------------------------------------- 
File Allocation Table
Contiene una entrada por cada bloque del filesystem, representada por un dato de tipo uint32_t, por
lo que su tamaño será la cantidad de bloques por sizeof(uint32_t).

Cada entrada indica cuál es el bloque siguiente del Checkpoint al que pertenece. De sus 32 bits sólo
los 28 menos significativos forman el número de bloque: los 4 más significativos se encuentran
reservados.

El valor 0 representa que el bloque se encuentra libre, y el valor 0x0FFFFFFF representa el fin del Checkpoint.

Las entradas correspondientes a bloques reservados deberán marcarse como 0x0FFFFFFE, de manera que nunca se los considere libres.

La tabla ocupará tantos bloques como sea necesario para almacenar sus entradas, completando con ceros el resto del último bloque en caso de no completarlo.
------------------------------------------------------------------------------------------------- 
Directorio
Contiene una entrada por cada Checkpoint (archivo) que exista en el filesystem, ocupando la región
de manera contigua desde su primer byte. Cada entrada deberá contar exactamente con la siguiente
estructura de datos:

●​Estado de la entrada: 1 byte (uint8_t, 0 si se encuentra libre y 1 si se encuentra ocupada).
●​Nombre del Checkpoint: 23 bytes (cadena de caracteres terminada en '\0').
●​Tamaño del Checkpoint expresado en bytes: 4 bytes (uint32_t).
●​Primer bloque de datos: 4 bytes (uint32_t).

En caso de que el directorio se encuentre lleno, deberá informarse con un error.
------------------------------------------------------------------------------------------------- 
Operaciones
El Storage deberá atender las siguientes operaciones, que serán solicitadas por el Planificador.
-------------------------------------------------------------------------------------------------
Guardar un Checkpoint
Se recibirá el nombre de un Checkpoint junto con el contenido a persistir. Si el Checkpoint no existe
se deberá crear su entrada en el directorio, y si ya existe su contenido anterior será reemplazado.
Se le deberán asignar tantos bloques como sean necesarios para direccionar el tamaño recibido y
liberar aquellos que dejen de ser necesarios.
En caso de no contar con espacio suficiente, se deberá informar dicha situación al Planificador y se finalizará el Job.
-------------------------------------------------------------------------------------------------
Cargar un Checkpoint
Se recibirá el nombre de un Checkpoint y se deberá responder su contenido completo. En caso de
que el Checkpoint no exista, se deberá informar dicha situación al Planificador y se finalizará el Job.
-------------------------------------------------------------------------------------------------
Eliminar un Checkpoint
Se recibirá el nombre de un Checkpoint y se deberá liberar el espacio que ocupaba: su entrada del
directorio deberá marcarse como libre, y todos los bloques de su cadena deberán marcarse como
libres en la tabla FAT.
En caso de que el Checkpoint no exista, se deberá informar dicha situación al Planificador, pero no se
debe finalizar el Job.
-------------------------------------------------------------------------------------------------
Journaling
Con el objetivo de preservar la coherencia de los datos ante posibles fallas, el sistema de archivos
utilizará un sistema de registro (Journaling) para intentar garantizar la integridad de la información.
Para ello, las operaciones Guardar y Eliminar un Checkpoint deberán quedar asentadas en dicho
registro antes de aplicarse efectivamente, de manera que el módulo pueda recuperar un estado
consistente luego de una finalización abrupta.
-------------------------------------------------------------------------------------------------
El journal
El journal es un archivo real (distinto al archivo del volumen del filesystem) de escritura secuencial,
en el que se describen anticipadamente todas las operaciones a realizarse luego en el filesystem.
Por cada operación se registrarán todas las modificaciones que la integran, agrupadas en lo que
denominaremos un “asiento”.
Un asiento deberá contener un identificador único, las modificaciones concretas que la operación
produce (bloques de datos, entradas de la FAT y del directorio) con el contenido que queda en cada
una, y unas marcas que indican el grado de completitud de la operación que comprende.
-------------------------------------------------------------------------------------------------
Procedimiento de uso
-------------------------------------------------------------------------------------------------
Primer paso: escritura en el journal

Se escribirán en el journal todas las modificaciones que comprendan dicha operación. Una vez
completado el asiento, se agregará una marca de tipo “Commit” indicando el mismo como
completado.
El journal deberá escribirse utilizando fwrite seguido de fflush y fsync, de manera de garantizar que el
asiento se encuentre efectivamente en disco antes de continuar.
-------------------------------------------------------------------------------------------------
Segundo paso: escritura en filesystem

Una vez completado el paso anterior, se aplicarán efectivamente todas las modificaciones del asiento
recientemente completado en el sistema de archivos. Una vez aplicada toda la operación, se
agregará una nueva marca de tipo “Applied” indicando que el asiento está aplicado.
El uso de fflush y fsync en este paso es opcional, pero en caso de usarlos deberán poder desactivarse
por configuración.
-------------------------------------------------------------------------------------------------
Recuperación ante fallas

Al iniciar, el Storage deberá recorrer cronológicamente el journal en la búsqueda de operaciones que
hayan quedado inconclusas, accionando de la siguiente manera por cada asiento:

*Un asiento sin marca “commit” será descartado
*Un asiento con ambas marcas presentes (“commit” y “applied”) será descartado
*Un asiento con marca “commit”, pero sin marca “applied” será re-aplicado en el filesystem en su totalidad

Las operaciones deberán ser idempotentes (volver a aplicar un asiento que ya había sido aplicado no puede alterar el resultado).

Este procedimiento será evaluado finalizando el módulo de manera abrupta en medio de una operación y volviéndolo a iniciar.
-------------------------------------------------------------------------------------------------
Ejemplo de un Asiento en el Journal
Supongamos que se solicita la operación SAVE_CHECKPOINT modelo7, y el Storage decide asignarle
los bloques de datos 0x0000000F y 0x00000010, utilizando la tercera entrada de directorio. La
estructura de ese asiento en el Journal podría verse de la siguiente manera:
RECORD_STARTED: 1
OP_DIR: Directorio 3 - Checkpoint "modelo7" - Tamaño 60 - Bloque Inicial 0x0000000F
OP_FAT: Entrada 0x0000000F -> Valor 0x00000010
OP_FAT: Entrada 0x00000010 -> Valor 0x0FFFFFFF
OP_WRITE: Bloque 0x0000000F - Contenido: <...>
OP_WRITE: Bloque 0x00000010 - Contenido: <...>
RECORD_COMMITED 1
RECORD_APPLIED 1

Considerando el ejemplo provisto, se aclara que la estructura definitiva del journal quedará a criterio
de cada grupo, pero deberá respetarse el formato de un archivo de texto.
Comandos locales
El Storage deberá implementar una consola por stdin, soportando los siguientes comandos. Estos
responderán mostrando información en un formato “human-readable”:

INFO: mostrar el contenido actual del superbloque

LS: listar todas las entradas de directorio, con todos sus atributos

CAT <CheckpointName> <cantBytes>: mostrar el contenido de los primeros “cantBytes” en hexadecimal

WRITE <CheckpointName> <hexaContent>: escribe o crea un archivo con el contenido especificado en hasta 20 bytes hexadecimales

RM <CheckpointName>: borra el archivo especificado

Se podrán agregar otros comandos que se consideren necesarios, siempre y cuando no modifiquen
ni contradigan el funcionamiento detallado en el módulo.






*/
