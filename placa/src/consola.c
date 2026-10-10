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
    char *leido = NULL;
    size_t tamanio = 0;

    if (getline(&leido, &tamanio, stdin) == -1)
    {
        free(leido);
        return NULL;
    }

    leido[strcspn(leido, "\n")] = '\0';

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
    		return false;

        default:
            fprintf(stderr, "Error: opción invalida\n");
            break;
    }

    return true;
}


