#include "estructuras_administrativas.h"

t_codigo_proceso * crear_codigo_proceso (uint32_t jid, char * path_job)
{
	t_codigo_proceso * nuevo_codigo_proceso = (t_codigo_proceso *) malloc (sizeof(t_codigo_proceso));
	
	nuevo_codigo_proceso->jid = jid;

	char * path_instrucciones_job = string_duplicate (path_instrucciones);
	string_append (&path_instrucciones_job, path_job);
	
	FILE *archivo = fopen(path_instrucciones_job, "r");
	//printf ("%s\n", path_instrucciones_job);
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

t_tabla_de_pagina * crear_tabla_paginas_proceso (uint32_t jid)
{
	t_tabla_de_pagina * nueva_tabla = (t_tabla_de_pagina *) malloc (sizeof(t_tabla_de_pagina));
	
	if (nueva_tabla == NULL) 
		return NULL; 
	
	nueva_tabla->jid = jid;
	nueva_tabla->tabla = list_create ();
	
	return nueva_tabla;
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

void destruir_tabla_de_pagina (void* elemento) 
{
    t_tabla_de_pagina* nodo = (t_tabla_de_pagina*) elemento;
    // Asumiendo que los elementos dentro de 'tabla' también necesitan liberación
    list_destroy_and_destroy_elements(nodo->tabla, free); 
    free(nodo);
}

void imprimir_instrucciones (void *elemento)
{
    t_codigo_proceso *codigo_proceso = elemento;

	printf("###########################\n");
	
    printf("PID: %u\n", codigo_proceso->jid);
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
	
	memoria_datos = malloc (tam_memoria);
	return memoria_datos;
}

FILE * crear_offload (int tamano_offload) 
{
    // Usamos "wb+" para que si ya existe lo limpie y empiece de cero, permite leer y escribir en el.
    FILE * ptr_file = fopen (path_offload, "wb+");
    
    if (ptr_file == NULL) 
    {
        fprintf(stderr, "Error: No se puede acceder a la ruta: %s\n", path_offload);
        exit(1);
    }

    // fileno() obtiene el Descriptor de Archivo (int) a partir del FILE *
    // ftruncate trunca o extiende el archivo al tamano exacto
    if ( ftruncate(fileno(ptr_file), tamano_offload ) == -1) 
    {
        fprintf(stderr, "Error al truncar el archivo Offload a su tamaño.\n");
        fclose(ptr_file);
        exit(EXIT_FAILURE);
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
uint32_t asignar_espacio_job (t_control_job * job, uint32_t bytes_a_asignar) 
{
    
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

int obtener_bloque_libre_offload (void) 
{
    for (int i = 0; i < cantidad_bloques_totales; i++) 
    {
        // CORRECCIÓN: bitarray_test_bit devuelve true si el bit está en 1, false si está en 0
        if (bitarray_test_bit(bitmap_offload, i) == false) 
        { 
            bitarray_set_bit(bitmap_offload, i); // Lo marcamos como ocupado (1)
            return i; 
        }
    }
    return -1; // Offload lleno
}



void liberar_bloque_offload(uint32_t nro_bloque) 
{
    bitarray_clean_bit(bitmap_offload, nro_bloque); // Pone el bit en 0 (libre)
}

void* crear_entrada_pagina (uint32_t bloque_offload) 
{
    void * entrada = NULL;

    if (strcmp(algoritmo_reemplazo, "CLOCK-M") == 0) 
    {
        entrada = malloc(sizeof(t_entrada_clock_m));
        if (entrada == NULL) return NULL;
        
        t_entrada_clock_m * p = (t_entrada_clock_m*) entrada;
        p->marco = 0;
        p->presencia = 0;
        p->uso = 0;
        p->modificado = 0;
        p->lock = 0;
        p->pos_offload = bloque_offload; // 👈 Guardamos el bloque asignado
    } 
    else if (strcmp(algoritmo_reemplazo, "LRU") == 0) 
    {
        entrada = malloc(sizeof(t_entrada_lru));
        if (entrada == NULL) return NULL;
        
        t_entrada_lru * p = (t_entrada_lru*) entrada;
        p->marco = 0;
        p->timestamp = 0;
        p->presencia = 0;
        p->modificado = 0;
        p->lock = 0;
        p->pos_offload = bloque_offload; // 👈 Guardamos el bloque asignado
    }
    
    return entrada; 
}


int contar_marcos_libres(void) 
{
    int contador_libres = 0;
    
    for (int i = 0; i < marcos_totales; i++) 
    {
        if (vector_de_marcos_memoria[i].esta_libre == true) 
        {
            contador_libres++;
        }
    }
    
    return contador_libres;
}

int contar_bloques_ocupados_offload(void) 
{
    int contador_ocupados = 0;
    
    // Recorremos usando el límite físico exacto que calculamos antes
    for (int i = 0; i < cantidad_bloques_totales; i++) 
    {
        // Si test_bit devuelve true, significa que el bloque está OCUPADO (tiene un 1)
        if (bitarray_test_bit(bitmap_offload, i) == true) 
        {
            contador_ocupados++;
        }
    }
    
    return contador_ocupados;
}

int contar_marcos_lockeados (void) 
{
    int contador_lockeados = 0;
    for (int i = 0; i < marcos_totales; i++) 
    {
        if (vector_de_marcos_memoria[i].esta_libre == false/*Esta ocupado*/&& vector_de_marcos_memoria[i].esta_lockeado == true) 
        {
            contador_lockeados++;
        }
    }
    return contador_lockeados;
}

