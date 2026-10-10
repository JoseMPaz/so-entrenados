#include "estructuras_administrativas.h"

t_codigo_proceso * crear_codigo_proceso (uint32_t pid, char * path_job)
{
	t_codigo_proceso * nuevo_codigo_proceso = (t_codigo_proceso *) malloc (sizeof(t_codigo_proceso));
	
	nuevo_codigo_proceso->pid = pid;

	char * path_instrucciones_job = string_duplicate (path_instrucciones);
	string_append (&path_instrucciones_job, path_job);
	
	FILE *archivo = fopen(path_instrucciones_job, "r");
	printf ("%s\n", path_instrucciones_job);
	free (path_instrucciones_job);

	
	if (archivo == NULL)//No pudo abrir el archivo
    {
        fprintf(stderr, "%s\n", "Error al abrir el archivo");
        free(nuevo_codigo_proceso);
        return NULL;
    }
	
	nuevo_codigo_proceso->instrucciones = list_create();
	
	char *linea = NULL;
    size_t tamanio = 0;
    ssize_t leidos;

    while ((leidos = getline(&linea, &tamanio, archivo)) != -1)
    {
        // Se elimina el '\n' que getline deja al final
        if (leidos > 0 && linea[leidos - 1] == '\n')
            linea[leidos - 1] = '\0';

        list_add(nuevo_codigo_proceso->instrucciones, string_duplicate(linea));
    }

    free(linea);
    fclose(archivo);
		
	return nuevo_codigo_proceso;
}

void destruir_codigo_proceso(void *elemento)
{
    if (elemento == NULL)
        return;

    t_codigo_proceso *codigo_proceso = elemento;

    list_destroy_and_destroy_elements(
        codigo_proceso->instrucciones,
        free
    );

    free(codigo_proceso);
}

void imprimir_instrucciones (void *elemento)
{
    t_codigo_proceso *codigo_proceso = elemento;

	printf("###########################\n");
	
    printf("PID: %u\n", codigo_proceso->pid);
    printf("Instrucciones:\n");

    for (int i = 0; i < list_size(codigo_proceso->instrucciones); i++)
    {
        char * instruccion = list_get(codigo_proceso->instrucciones, i);
        printf("\t%d: %s\n", i, instruccion);
    }

    printf("###########################\n");
}

void * crear_memoria_de_datos (void)
{
	void * memoria_datos;
	
	memoria_datos = (void *) malloc (tam_memoria);
	return memoria_datos;
}

FILE * crear_offload (void)
{
	// "ab+" permite leer y escribir al final en modo binario (creando el archivo si no existe)
	FILE * ptr_file = fopen (path_offload, "ab+");
	
	if (ptr_file == NULL)
	{
		fprintf (stderr, "%s%s\n", "Error: No se puede acceder a la ruta: ", path_offload);
		exit(1);
	}
	return ptr_file;
}

void destruir_offload (FILE * ptr_file)
{
	if (ptr_file != NULL)
		fclose (ptr_file);
	return;
}

// Retorna la dirección lógica donde comienza la nueva asignación
uint32_t asignar_espacio_job(t_control_job* job, uint32_t bytes_a_asignar) {
    
    // 1. Calcular cuántas páginas enteras se necesitan (redondeo hacia arriba)
    uint32_t paginas_necesarias = (bytes_a_asignar + tam_pagina - 1) / tam_pagina;
    
    // [AQUÍ DEBÉS VALIDAR SI HAY ESPACIO SUFICIENTE EN EL OFFLOAD GENERAL]
    // Si no hay espacio, informás el error al Planificador.

    // 2. La nueva asignación SIEMPRE arranca al inicio de una página nueva.
    // Como la tabla es un t_list, el número de la próxima página es el tamaño actual de la lista.
    uint32_t pagina_inicial_nueva = list_size(job->tabla_paginas);
    uint32_t dir_logica_resultado = pagina_inicial_nueva * tam_pagina;

    // 3. Crear e insertar las nuevas páginas en la tabla del Job
    for (int i = 0; i < paginas_necesarias; i++) {
        t_entrada_pagina* nueva_pagina = malloc(sizeof(t_entrada_pagina));
        
        nueva_pagina->nro_marco = 0;          // No tiene marco físico asignado aún
        nueva_pagina->bit_presencia = false;  // Paginación bajo demanda (arranca en false)
        nueva_pagina->bit_modificado = false;
        nueva_pagina->bit_uso = false;
        
        // Asignás el lugar que le corresponde en tu archivo de Offload
        nueva_pagina->pos_offload = obtener_bloque_libre_offload(); 

        // Se agrega al final de la lista
        list_add(job->tabla_paginas, nueva_pagina);
    }

    // Retornás la dirección lógica donde empieza esta ráfaga de páginas
    return dir_logica_resultado;
}

int obtener_bloque_libre_offload() 
{
    for (int i = 0; i < bloques_totales_offload; i++) {
        // CORRECCIÓN: bitarray_test_bit devuelve true si el bit está en 1, false si está en 0
        if (bitarray_test_bit(bitarray_offload, i) == false) { 
            bitarray_set_bit(bitarray_offload, i); // Lo marcamos como ocupado (1)
            return i; 
        }
    }
    return -1; // Offload lleno
}


void inicializar_offload(uint32_t tam_offload, uint32_t tam_pagina) 
{
    bloques_totales_offload = tam_offload / tam_pagina;
    
    size_t bytes_bitarray = (bloques_totales_offload + 7) / 8;
    char* estructura_bits = malloc(bytes_bitarray);
    memset(estructura_bits, 0, bytes_bitarray); 

    // CORRECCIÓN: Usamos bitarray_create_with_mode apuntando a LSB_FIRST
    bitarray_offload = bitarray_create_with_mode(estructura_bits, bytes_bitarray, LSB_FIRST);
}

void liberar_bloque_offload(uint32_t nro_bloque) 
{
    bitarray_clean_bit(bitarray_offload, nro_bloque); // Pone el bit en 0 (libre)
}


