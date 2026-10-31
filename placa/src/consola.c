#include "consola.h"

void *consola(void *arg)
{
    char *leido;

    while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST))
    {
        mostrar_consola();

        leido = leer_consola();

        if (salir)
        {
            free(leido);
            break;
        }

        if (leido == NULL)
            break;

        if (hacer_consola(leido) == false)
        {
            free(leido);
            break;
        }

        free(leido);
    }

    return NULL;
}

void mostrar_consola (void)
{
	printf ("%s\n", " ********** Menu *********");
	printf ("%s\n", "1) INFO"); 
	printf ("%s\n", "2) TLS");
	printf ("%s\n", "3) EXIT");
	return;
}

char *leer_consola(void)
{
    char buffer[100];

    // fgets lee hasta 99 caracteres o hasta encontrar un '\n'
    // Si se presiona Ctrl+C, fgets se interrumpe y devuelve NULL
    if (fgets(buffer, sizeof(buffer), stdin) == NULL)
    {
        return NULL;
    }

    // Limpiamos el salto de línea '\n' si está presente
    buffer[strcspn(buffer, "\n")] = '\0';

    // Pedimos memoria dinámica del tamaño justo y copiamos el contenido
    // string_duplicate de las Commons hace el malloc + strcpy internamente.
    // Si preferís C puro: char *leido = strdup(buffer);
    char *leido = string_duplicate(buffer);

    return leido;
}

bool hacer_consola(char *leido)
{
    char *ultimo_caracter_leido = NULL;
    int marcos_libres;
    int marcos_lockeados;
    int marcos_ocupados;
    double porcentaje_memoria_ocupada;
    int bloques_ocupados;
    double porcentaje_offload_ocupado; 
    int jobs_actuales;

    if (strlen(leido) == 0)
    {
        fprintf(stderr, "Error: opción invalida\n");
        return true;
    }

    int opcion = strtol(leido, &ultimo_caracter_leido, 10);

    if (*ultimo_caracter_leido != '\0')
    {
        fprintf(stderr, "Error: opción invalida\n");
        return true;
    }

    switch (opcion)
    {
        case 1:
        	printf(" ########## INFO #########\n");
            marcos_libres = contar_marcos_libres ();
            marcos_lockeados = contar_marcos_lockeados ();
            marcos_ocupados = marcos_totales - marcos_libres;
            porcentaje_memoria_ocupada = ( (double) marcos_ocupados / marcos_totales) * 100.0;
            bloques_ocupados = contar_bloques_ocupados_offload();
            porcentaje_offload_ocupado = ((double)bloques_ocupados / cantidad_bloques_totales) * 100.0;
            jobs_actuales = list_size (instrucciones_de_jobs);


            printf("Porcentaje de memoria ocupada: %6.2f %%\n", porcentaje_memoria_ocupada);
            printf("Porcentaje de offload ocupado: %6.2f %%\n", porcentaje_offload_ocupado);
            printf("Cantidad de marcos libres: %d\n", marcos_libres);
            printf("Cantidad de marcos lockeados: %d\n", marcos_lockeados);
            printf("Cantidad de Jobs actuales: %d\n", jobs_actuales);
            printf("Cantidad de marcos totales: %d\n", marcos_totales);
            printf("Cantidad total de bloques: %d\n", cantidad_bloques_totales);    
            printf("Cantidad total de marcos: %d\n", marcos_totales); 
            printf("Tamaño de la memoria de datos: %d [B]\n", tam_memoria);
            printf("Tamaño del offload: %d [B]\n", tam_offload);
            printf("Tamaño de pagina:   %d [B]\n", tam_pagina);
            printf("Tamaño de marco:   %d [B]\n", tam_marco);
            printf("Tamaño de bloque:   %d [B]\n", tam_bloque);
            break;

        case 2:
            printf(" ########## TLS #########\n");
            break;
        
        case 3:
        	printf(" ########## EXIT #########\n");
    		printf("Modulo Placa finaliza ...\n");
    		
    		return false;

        default:
            fprintf(stderr, "Error: opción invalida\n");
            break;
    }

    return true;
}


