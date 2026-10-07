#include <utils/hello.h>
#include <utils/cla.h>

int main(int argc, char* argv[]) 
{
	t_validar_argumentos validar_argumentos = validar_dos_argumento;
	validar_argumentos (argc, argv);
    saludar("core");
    validar_argumentos (argc, argv);
    
    return 0;
}

/*
Módulo: Core

Este módulo se encarga de interpretar y ejecutar las instrucciones de los Jobs. 
Para ello, ejecutará un ciclo de instrucción simplificado que cuenta con los pasos: 

Fetch, Decode, Execute y Check Interrupt.

Lineamiento e Implementación
Cada Core contará con un ID propio, recibido por parámetro.

> ./bin/core [Archivo Config] [ID]

Para cada Core se deberá tener:
 un archivo de log y 
 un archivo de configuración independientes, 
 que deberán contar con el ID pasado por parámetro para poder identificar a qué Core
corresponde el archivo de log que se está leyendo. 

Los Cores deberán conectarse al Planificador y a la Placa.

Una vez establecidas todas las conexiones, el Core se quedará a la espera de recibir de parte del
Planificador un JID junto con su contexto de ejecución, para comenzar el primer ciclo de instrucción.

A la hora de ejecutar instrucciones que requieran interactuar directamente con la memoria, tendrá
que traducir las direcciones lógicas a direcciones físicas. Para ello usará la MMU.
------------------------------------------------------------------------------------------------- 
Registros del Core

PC 
P0, P1, P2, P3, P4, P5
AX, BX, CX, DX
E1, E2, E3
------------------------------------------------------------------------------------------------- 
Ciclo de Instrucción
------------------------------------------------------------------------------------------------- 
Fetch
La primera etapa del ciclo consiste en buscar la próxima instrucción a ejecutar. En este trabajo
práctico cada instrucción deberá ser pedida a la Placa utilizando el Program Counter que representa
el número de instrucción a buscar relativo al Job en ejecución.
------------------------------------------------------------------------------------------------- 
Decode
Esta etapa consiste en interpretar qué instrucción es la que se va a ejecutar y si la misma requiere de
una traducción de dirección lógica a dirección física.
------------------------------------------------------------------------------------------------- 
MMU
A la hora de traducir direcciones lógicas a físicas (ambas expresadas siempre en decimal), el Core
debe tomar en cuenta que el esquema de la memoria de este sistema es de Paginación, por lo tanto,
las direcciones lógicas se interpretarán de la siguiente manera:

[nro_pagina | desplazamiento]

Las fórmulas para extraer ambos componentes de una DL serán las siguientes:

nro_pagina = floor ( dir_logica/tamaño_pagina )
desplazamiento = dir_logica % tamaño_pagina

Es posible que una petición de lectura o escritura abarque más de una página. Será responsabilidad
del grupo dividir estas peticiones de acuerdo a las diferentes páginas involucradas.

Todas las traducciones que requiera una instrucción deberán resolverse antes de modificar la
memoria o los registros del Job.

En caso de que la página a la que se quiere acceder no se encuentre en la memoria de la Placa, se
deberá devolver el Job al Planificador con motivo de Page Fault, sin actualizar el Program Counter.

Las páginas obtenidas quedarán lockeadas en la Placa, por lo que el Core deberá informar su
liberación al finalizar la instrucción o al devolver el Job al Planificador. 

Se exceptúan las Syscalls que operan sobre la memoria del Job, cuyas páginas quedarán lockeadas 
hasta que el Planificador informe su liberación.
------------------------------------------------------------------------------------------------- 
Execute
En este paso se deberá ejecutar lo correspondiente a cada instrucción:

Instrucciones básicas:

●​ NOOP: Representa la instrucción No Operation, es decir, va a consumir solamente el tiempo
del ciclo de instrucción.
●​ SET (Registro, Valor): Asigna al registro el valor pasado como parámetro, que podrá ser un
número o el contenido de otro registro.
●​ SUM (Registro, Valor): Suma al registro el valor pasado como parámetro.
●​ SUB (Registro, Valor): Resta al registro el valor pasado como parámetro.
●​ JNZ (Registro, Instrucción): Si el valor del registro es distinto de cero, actualiza el program
counter al número de instrucción pasada por parámetro.

Instrucciones de entrenamiento:

Las tres próximas instrucciones hacen referencia a operaciones que se utilizan para el entrenamiento,
pero para este trabajo práctico su implementación se representa con operaciones aritméticas
simples sobre enteros de 4 bytes sin signo (uint32_t). Los registros E1, E2 y E3 se corresponden con
los tres primeros parámetros de la instrucción, y el Core cargará en ellos el contenido de las
direcciones apuntadas antes de ejecutar dicha instrucción. Solo deberá cargar los registros que esa
instrucción vaya a consumir (leer).

●​ FORWARD (Registro Pesos, Registro Entrada, Registro Salida, Registro Acumulador)

acumulador = acumulador + E1 * E2
E3 = acumulador

●​ BACKWARD (Registro Pesos, Registro Activaciones, Registro Gradientes, Registro Acumulador)

acumulador = acumulador - E1 * E2
E3 = acumulador

●​ UPDATE (Registro Gradientes, Registro Estado, Registro Pesos)

E2 = (E2 + E1) / 2
E3 = E3 - E2

Finalizada la ejecución, los registros de entrenamiento que hayan sido modificados (escritos) en esa
instrucción deberán escribirse en la memoria del Job.
------------------------------------------------------------------------------------------------- 
Instrucciones de tipo syscall
Las siguientes instrucciones se considerarán Syscalls, ya que las mismas no pueden ser resueltas por
el Core y depende de la acción del Planificador para su realización. A diferencia de la vida real donde
la llamada es a una única instrucción, para simplificar la comprensión de los scripts, vamos a utilizar
un nombre diferente para cada Syscall. La descripción de lo que hace cada una va a estar detallada
en el Planificador.

Las syscalls, al depender del Planificador, deberán ser solicitadas al mismo enviándole el JID y los
parámetros correspondientes. Aquellas que bloqueen al Job deberán además liberar el Core y
devolver el contexto de ejecución. 

Cuando la Syscall opere sobre la memoria del Job, el Core deberá
traducir el rango indicado antes de devolver el Job y enviar al Planificador las direcciones físicas
resultantes.


INIT_JOB, ALLOC y FREE no bloquean al Job: el Core deberá esperar la respuesta del Planificador y
continuar con el siguiente ciclo de instrucción. En el caso de ALLOC, deberá guardar en el Registro
Dirección Lógica la dirección recibida.

●​ INIT_JOB (Archivo de instrucciones)
●​ ALLOC (Registro Dirección Lógica, Tamaño)
●​ FREE (Registro Dirección Lógica)
●​ LOAD_BATCH (Nombre de archivo, Registro Dirección Lógica, Registro Tamaño)
●​ LABEL (Registro Dirección Lógica, Registro Tamaño)
●​ REPORT (Registro Dirección Lógica, Registro Tamaño)
●​ SAVE_CHECKPOINT (Nombre, Registro Dirección Lógica, Registro Tamaño)
●​ LOAD_CHECKPOINT (Nombre, Registro Dirección Lógica, Registro Tamaño)
●​ DELETE_CHECKPOINT (Nombre)
●​ EXIT

Es importante tener en cuenta que, al finalizar el ciclo de instrucción, el Program Counter (PC) deberá
ser actualizado sumándole 1, siempre y cuando este no haya sido modificado por la instrucción
ejecutada anteriormente.
------------------------------------------------------------------------------------------------- 
Check Interrupt
En este momento, se deberá chequear si el Planificador nos envió una interrupción al Job que se está
ejecutando. En caso afirmativo, se le devuelve el JID y el contexto de ejecución al Planificador con el
motivo de la interrupción. Caso contrario, continúa con el siguiente ciclo de instrucción.

Ejemplo de un Job que realiza un paso de entrenamiento:
ALLOC P0 640
SET CX 128
SET P1 P0
SUM P1 128
LOAD_BATCH lote1 P1 CX
SET P1 P0
LOAD_CHECKPOINT modelo7 P1 CX
SET AX 0
SET P1 P0
SET P2 P0
SUM P2 128
SET P3 P0
SUM P3 256
SET DX 32
FORWARD P1 P2 P3 AX
SUM P1 4
SUM P2 4
SUM P3 4
SUB DX 1
JNZ DX 15
SET BX 0
SET P1 P0
SUM P1 124
SET P2 P0
SUM P2 380
SET P3 P0
SUM P3 508
SET DX 32
BACKWARD P1 P2 P3 BX
SUB P1 4
SUB P2 4
SUB P3 4
SUB DX 1
JNZ DX 29
SET P1 P0
SUM P1 384
SET P2 P0
SUM P2 512
SET P3 P0
SET DX 32
UPDATE P1 P2 P3
SUM P1 4
SUM P2 4
SUM P3 4
SUB DX 1
JNZ DX 41
SET P1 P0
REPORT P1 CX
SAVE_CHECKPOINT modelo7 P1 CX
FREE P0
EXIT



*/
