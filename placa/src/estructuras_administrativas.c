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

