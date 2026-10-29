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
	printf ("%s\n", "*** Menu ***");
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
            printf("Seleccionaste INFO\n");
            break;

        case 2:
            printf("Seleccionaste TLS\n");
            break;
        
        case 3:
    		printf("Seleccionaste EXIT\n");
    		printf("Modulo Placa finaliza finaliza ...\n\n\n");
    		
    		return false;

        default:
            fprintf(stderr, "Error: opción invalida\n");
            break;
    }

    return true;
}


