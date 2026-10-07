# tp-scaffold

Esta es una plantilla de proyecto diseñada para generar un TP de Sistemas
Operativos de la UTN FRBA.

## Dependencias

Para poder compilar y ejecutar el proyecto, es necesario tener instalada la
biblioteca [so-commons-library] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-commons-library
cd so-commons-library
make debug
make install
```

## Compilación y ejecución

Cada módulo del proyecto se compila de forma independiente a través de un
archivo `makefile`. Para compilar un módulo, es necesario ejecutar el comando
`make` desde la carpeta correspondiente.

El ejecutable resultante de la compilación se guardará en la carpeta `bin` del
módulo. Ejemplo:

```sh
cd core
make
./bin/core
```

## Importar desde Visual Studio Code

Para importar el workspace, debemos abrir el archivo `tp.code-workspace` desde
la interfaz o ejecutando el siguiente comando desde la carpeta raíz del
repositorio:

```bash
code tp.code-workspace
```

## Checkpoint

Para cada checkpoint de control obligatorio, se debe crear un tag en el
repositorio con el siguiente formato:

```
checkpoint-{número}
```

Donde `{número}` es el número del checkpoint, ejemplo: `checkpoint-1`.

Para crear un tag y subirlo al repositorio, podemos utilizar los siguientes
comandos:

```bash
git tag -a checkpoint-{número} -m "Checkpoint {número}"
git push origin checkpoint-{número}
```

> [!WARNING]
> Asegúrense de que el código compila y cumple con los requisitos del checkpoint
> antes de subir el tag.

## Entrega

Para desplegar el proyecto en una máquina Ubuntu Server, podemos utilizar el
script [so-deploy] de la cátedra:

```bash
git clone https://github.com/sisoputnfrba/so-deploy.git
cd so-deploy
./deploy.sh -r=release -p=utils -p=planificador -p=core -p=placa -p=storage "tp-{año}-{cuatri}-{grupo}"
```

El mismo se encargará de instalar las Commons, clonar el repositorio del grupo
y compilar el proyecto en la máquina remota.

> [!NOTE]
> Ante cualquier duda, pueden consultar la documentación en el repositorio de
> [so-deploy], o utilizar el comando `./deploy.sh --help`.

## Guías útiles

- [Cómo interpretar errores de compilación](https://docs.utnso.com.ar/primeros-pasos/primer-proyecto-c#errores-de-compilacion)
- [Cómo utilizar el debugger](https://docs.utnso.com.ar/guias/herramientas/debugger)
- [Cómo configuramos Visual Studio Code](https://docs.utnso.com.ar/guias/herramientas/code)
- **[Guía de despliegue de TP](https://docs.utnso.com.ar/guías/herramientas/deploy)**

[so-commons-library]: https://github.com/sisoputnfrba/so-commons-library
[so-deploy]: https://github.com/sisoputnfrba/so-deploy

1. Responsabilidad de cada Módulo
• Planificador: Administra la cola de Jobs y gestiona el modelo de 5 estados (NEW, READY, EXEC, BLOCK, EXIT). Atiende de forma multihilo a los Cores y resuelve las llamadas al sistema (Syscalls) e interrupciones.
• Core: Simula la CPU. Ejecuta el ciclo de instrucción (Fetch, Decode, Execute, Check Interrupt), traduce direcciones lógicas a físicas mediante su MMU e interactúa con el Planificador cuando se encuentra con una Syscall o una interrupción.
• Placa: Administra el espacio total de memoria (Instrucciones y Datos de usuario). Implementa paginación bajo demanda, gestiona las Tablas de Páginas de cada Job y el archivo de Offload (Swap) para el reemplazo de páginas.
• Storage: Se encarga de persistir y recuperar los archivos de Checkpoint del sistema usando un sistema de archivos propio (FAT32_TRAIN) y asegurando la consistencia mediante un mecanismo de Journaling.
2. Flujo y Pasaje de Mensajes (Operadores y Operandos)
🔄 Inicialización y Ejecución del Ciclo de Instrucción (Base)
1. Carga del Job:
	• De Planificador ➜ Placa: Crear Job (Operandos: JID, Nombre de archivo de pseudocódigo).
	• De Placa ➜ Planificador: Confirmación de estructuras inicializadas.
2. Búsqueda de instrucción (Fetch):
	• De Core ➜ Placa: Obtener Instrucción (Operandos: JID, Program Counter (PC)).
	• De Placa ➜ Core: Instrucción devuelta (Operando: Cadena de la instrucción a ejecutar).
3. Traducción de Memoria (MMU) durante el Execute:
	• De Core ➜ Placa: Obtención de Marco (Operandos: JID, Número de Página).
	• De Placa ➜ Core (Caso Éxito): Responder Marco (Operando: Número de Marco) -> La página se marca como lockeada.
	• De Placa ➜ Core (Caso Fallo): Page Fault.
⚠️ Flujo ante un Page Fault
Cuando la Placa le avisa al Core que la página no está en memoria:
1. De Core ➜ Planificador: Notificar Page Fault (Operandos: JID, Contexto de ejecución actual sin actualizar el PC). El Core queda libre.
2. De Planificador ➜ Placa: Carga de Página (Operandos: JID, Número de Página). El Planificador bloquea al Job (BLOCK).
3. De Placa ➜ Planificador: Página Cargada (Luego de subirla desde el Offload, aplicando algoritmo de reemplazo si fuese necesario).
4. Planificador: Pasa el Job a READY para ser planificado nuevamente.
📞 Flujo de Syscalls No Bloqueantes (INIT_JOB, ALLOC, FREE)
El Core detecta la instrucción, la solicita al Planificador y espera la respuesta sin liberar la CPU.
• ALLOC:
	• De Core ➜ Planificador: Solicitar ALLOC (Operandos: JID, Tamaño en bytes).
	• De Planificador ➜ Placa: Asignar Espacio (Operandos: JID, Tamaño en bytes).
	• De Placa ➜ Planificador: Dirección Lógica donde comienza el espacio asignado (o código de error si no hay espacio en Offload).
	• De Planificador ➜ Core: Responder ALLOC (Operando: Dirección Lógica). El Core lo guarda en el registro de dirección lógica.
• FREE:
	• De Core ➜ Planificador: Solicitar FREE (Operandos: JID, Dirección Lógica).
	• De Planificador ➜ Placa: Liberar Espacio (Operandos: JID, Dirección Lógica).
	• De Placa ➜ Planificador ➜ Core: Confirmación de liberación.
🛑 Flujo de Syscalls Bloqueantes que operan sobre Memoria
(Aplica a LOAD_BATCH, LABEL, REPORT, SAVE_CHECKPOINT, LOAD_CHECKPOINT).
Como operan sobre la memoria del Job, el Core primero traduce todas las páginas involucradas (quedando lockeadas en la Placa) y desaloja el proceso enviándolo al Planificador.
• Ejemplo: SAVE_CHECKPOINT
	1. De Core ➜ Planificador: Ejecutar Syscall SAVE_CHECKPOINT (Operandos: JID, Contexto de ejecución, Nombre del Checkpoint, Tamaño, y una lista de pares [Dirección Física; Tamaño] traducidos por la MMU). El Core se libera.
	2. De Planificador ➜ Placa: Lectura de Datos (Operandos: JID, Dirección Física, Tamaño) iterando por cada página lockeada.
	3. De Placa ➜ Planificador: Contenido de la memoria (Operando: Stream de bytes leídos).
	4. De Planificador ➜ Storage: Guardar Checkpoint (Operandos: Nombre del Checkpoint, Contenido/Bytes a persistir). (El Storage procesará esto en su Journal y Filesystem).
	5. De Storage ➜ Planificador: Confirmación de Guardado.
	6. De Planificador ➜ Placa: Deslockeo de Páginas (Operandos: JID).
	7. De Placa ➜ Planificador: Confirmación de deslockeo. El Planificador mueve el Job de BLOCK a READY.
• Ejemplo: Servicios Internos (LOAD_BATCH para el Loader)
	1. De Core ➜ Planificador: Ejecutar Syscall LOAD_BATCH (Operandos: JID, Contexto, Nombre de archivo, Tamaño, lista de pares [Dirección Física; Tamaño]).
	2. Planificador (Loader): Lee el archivo localmente desde su PATH_DATOS.
	3. De Planificador ➜ Placa: Escritura de Datos (Operandos: JID, Dirección Física, Tamaño, Contenido/Bytes leídos del archivo).
	4. De Placa ➜ Planificador: Confirmación de escritura.
	5. De Planificador ➜ Placa: Deslockeo de Páginas (Operandos: JID). El Job vuelve a READY.
✂️ Fin de Quantum / Interrupciones (Check Interrupt)
Al finalizar cada instrucción, el Core verifica si hay una señal de desalojo del Planificador (por ejemplo, fin de Quantum en Round Robin).
1. De Planificador ➜ Core: Enviar Interrupción (Señal de desalojo de forma asincrónica).
2. Core (en el paso Check Interrupt): Detecta la señal.
3. De Core ➜ Planificador: Devolver Job por Interrupción (Operandos: JID, Contexto de ejecución actualizado). El Core se libera.
4. De Planificador ➜ Placa: Deslockeo de Páginas (Operandos: JID). El Job pasa al estado READY.

**********************************************************************************

🛠️ Profundización 1: División de Peticiones y MMU (Core ↔ Placa)
Cuando una instrucción (como un FORWARD o una syscall como LOAD_BATCH) opera sobre la memoria de usuario, el Core recibe una Dirección Lógica inicial y un Tamaño en bytes. Dado que el sistema usa un esquema de Paginación, el espacio dinámico del Job no es necesariamente contiguo en la memoria física (los marcos de la Placa pueden estar dispersos). Por lo tanto, el Core nunca puede enviar una solicitud lineal por todo el tamaño de golpe si este cruza el límite de una página.
Las Fórmulas Base de la MMU
Para cualquier dirección lógica (dir_logica), la MMU extrae dos componentes esenciales usando el tamaño de página (tam_pagina) configurado en el sistema:
• \(\text{nro\_pagina} = \lfloor \text{dir\_logica} / \text{tam\_pagina} \rfloor\)
• \(\text{desplazamiento} = \text{dir\_logica} \pmod{\text{tam\_pagina}}\)
El Algoritmo de División (Paso a Paso en el Core)
Para traducir un rango completo [Dirección Lógica Inicial; Tamaño Total], el Core ejecuta un ciclo en su MMU para generar una lista de pares [Dirección Física; Tamaño Parcial]:
1. Calcular el espacio restante de la página actual:
\(\text{bytes\_disponibles\_en\_pagina}=\text{tam\_pagina}-\text{desplazamiento}\)
2. Determinar cuántos bytes operar en esta iteración:
\(\text{bytes\_a\_operar}=\min (\text{tamaño_restante_peticion},\text{bytes\_disponibles\_en\_pagina})\)
3. Solicitar el Marco a la Placa: El Core le envía a la Placa el JID y el nro\_pagina actual.
	• Si la Placa responde con un Marco: Se calcula la dirección física de esta iteración:
\(\text{dir\_fisica}=(\text{nro\_marco}\times \text{tam\_pagina})+\text{desplazamiento}\)
	• Si la Placa responde con Page Fault: Se aborta la traducción inmediatamente, no se modifican registros y se devuelve el Job al Planificador.
4. Actualizar el bucle:
	• Se guarda el par [dir_fisica; bytes_a_operar] en la lista que se enviará a la syscall.
	• \(\text{tamaño\_restante\_peticion} = \text{tamaño\_restante\_peticion} - \text{bytes\_a\_operar}\)
	• \(\text{nro\_pagina} = \text{nro\_pagina} + 1\)
	• \(\text{desplazamiento} = 0\) (ya que las siguientes páginas se leen desde el principio).
5. Repetir hasta que \(\text{tamaño\_restante\_peticion} == 0\).
📊 Ejemplo Práctico de Traducción y División
Supongamos la siguiente configuración en el archivo de la Placa:
• TAM_PAGINA = 64 bytes
• La instrucción es: LOAD_BATCH lote1 P1 CX
• El registro P1 (Dirección Lógica) vale 100
• El registro CX (Tamaño a cargar) vale 80 bytes
El Core debe traducir el rango [100; 80]:
• Iteración 1 (Página Inicial):
	• \(\text{nro\_pagina} = \lfloor 100 / 64 \rfloor = \mathbf{1}\)
	• \(\text{desplazamiento} = 100 \pmod{64} = \mathbf{36}\)
	• \(\text{bytes\_disponibles} = 64 - 36 = \mathbf{28}\) bytes.
	• \(\text{bytes\_a\_operar} = \min(80, 28) = \mathbf{28}\) bytes.
	• El Core le pide a la Placa el marco de la Página 1. Supongamos que la Placa dice que está en el Marco 5.
	• \(\text{dir\_fisica\_1} = (5 \times 64) + 36 = \mathbf{356}\).
	• Resultado 1: [356; 28]
• Iteración 2 (Siguiente Página):
	• \(\text{tamaño\_restante} = 80 - 28 = \mathbf{52}\) bytes.
	• \(\text{nro\_pagina} = 1 + 1 = \mathbf{2}\).
	• \(\text{desplazamiento} = \mathbf{0}\).
	• \(\text{bytes\_disponibles} = 64 - 0 = \mathbf{64}\) bytes.
	• \(\text{bytes\_a\_operar} = \min(52, 64) = \mathbf{52}\) bytes.
	• El Core le pide a la Placa el marco de la Página 2. Supongamos que está en el Marco 9.
	• \(\text{dir\_fisica\_2} = (9 \times 64) + 0 = \mathbf{576}\).
	• Resultado 2: [576; 52]
Al finalizar, la MMU dividió la petición en dos tramos: [356; 28] y [576; 52]. Como es una syscall, estas páginas quedarán lockeadas en la Placa hasta que el Planificador termine de operar con ellas.
💾 Profundización 2: El Mecanismo de Journaling (Storage)
El Journaling en el Storage implementa un protocolo de Write-Ahead Logging (WAL) de tipo Redo Log. Su objetivo básico es evitar la corrupción del sistema de archivos (FAT32_TRAIN) ante cortes de energía o caídas abruptas (kill -9) en medio de una escritura masiva.
El Journal es un archivo secuencial de texto plano totalmente independiente del binario que simula el disco duro físico.
El Protocolo de Dos Pasos para SAVE_CHECKPOINT
Cuando el Planificador le pide al Storage guardar un Checkpoint, este no toca el Filesystem directamente. Sigue estrictamente este orden:
[ Planificador ] ➔ (Solicitud) ➔ [ STORAGE ]
                                    │
    ┌───────────────────────────────┴──────────────────────────────┐
    ▼ Paso 1: Escribir en Journal                                  ▼ Paso 2: Escribir en Filesystem
┌───────────────────────────────┐                              ┌───────────────────────────────┐
│ 1. RECORD_STARTED             │                              │ 1. Aplicar cambios reales en: │
│ 2. Describir OP_DIR           │                              │    - Directorio               │
│ 3. Describir OP_FAT           │                              │    - Tabla FAT                │
│ 4. Describir OP_WRITE (datos) │                              │    - Bloques de datos         │
│ 5. RECORD_COMMITTED           │                              │ 2. RECORD_APPLIED             │
└───────────────┬───────────────┘                              └───────────────┬───────────────┘
                ▼                                                             						     ▼
         (fwrite + fflush + fsync)                                     (fflush + fsync opcional)
Paso 1: Asegurar la intención en el Journal 📝
1. Abre el archivo de journal y escribe la marca RECORD_STARTED: <ID>.
2. Escribe las líneas de metadatos de lo que planea hacer (OP_DIR, OP_FAT) y el contenido crudo de los datos en formato Hexadecimal (OP_WRITE).
3. Escribe la marca crítica RECORD_COMMITTED <ID>.
4. Obligatorio por diseño: Llama de forma sucesiva a fwrite(), fflush() y fsync(). Esto último fuerza al sistema operativo anfitrión (Linux) a vaciar los buffers del búfer del kernel e impactar físicamente los bytes en el disco duro antes de avanzar.
Paso 2: Ejecutar los cambios en el volumen real 💿
1. Modifica las estructuras del filesystem real en memoria/disco (escribe la entrada del directorio, actualiza la cadena de bloques en la tabla FAT y vuelca los bytes de datos en los bloques correspondientes).
2. Escribe en el Journal la marca final RECORD_APPLIED <ID> para dar por cerrada la transacción exitosamente.
El Proceso de Recuperación ante Fallas (Crash Recovery)
Cada vez que el Storage se inicia (ya sea en un arranque limpio o tras haber sufrido un cierre inesperado por un kill -9), su primera tarea obligatoria es leer cronológicamente de principio a fin el archivo de Journal para comprobar la consistencia.
Al escanear cada asiento (ID), el Storage evalúa la presencia de las etiquetas de control para decidir qué acción tomar:
Estado de Marcas en el Asiento	Significado Teórico	Acción del Storage al Iniciar
STARTED presente
❌ Sin COMMITTED
❌ Sin APPLIED	Falla durante el Paso 1: El sistema se cayó mientras escribía el Journal. Los datos del Checkpoint están incompletos en el log.	DESCARTAR 🗑️
Se ignora el asiento por completo. El filesystem se mantiene intacto tal como estaba antes de iniciar la operación fallida.
STARTED presente
✅ COMMITTED presente
❌ Sin APPLIED	Falla durante el Paso 2: Los datos se guardaron de forma 100% segura en el Journal, pero el sistema se interrumpió antes de escribirlos (o terminarlos) en el Filesystem.	RE-APLICAR (Redo) 🔄
El Storage lee los operandos (OP_DIR, OP_FAT, OP_WRITE) del Journal e impacta manualmente los cambios en el filesystem. Al terminar, escribe la marca RECORD_APPLIED faltante.
STARTED presente
✅ COMMITTED presente
✅ APPLIED presente	Operación Exitosa: Todo el flujo se completó de manera correcta antes del apagado.	DESCARTAR 👌
No se hace nada dado que los cambios ya se encuentran reflejados de forma nativa en el volumen.
Idempotencia: La Regla de Oro
El enunciado aclara que la re-aplicación debe ser idempotente. Esto significa que si el Storage se cayó en el Paso 2 habiendo escrito la mitad de los bloques de datos en el filesystem, al volver a iniciar va a sobreescribir todos los bloques listados en el asiento desde cero. Ejecutar esta acción una o diez veces genera exactamente el mismo resultado final y consistente, garantizando que el sistema nunca quede en un estado híbrido o corrupto.
