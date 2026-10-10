#include "estructuras_administrativas.h"

// Función auxiliar interna para liberar la memoria de un nodo t_core
void destruir_nodo_core(void * elemento) 
{
	t_core* core = (t_core*) elemento;
	if (core != NULL) 
	{
		if (core->socket != -1) 
		{
			close(core->socket); // Cierra el socket si quedó abierto
		}
		free(core); // Libera el espacio en memoria del struct
	}
}

// Función principal para destruir la lista completa de forma segura
void destruir_lista_cores (t_list* lista_a_destruir) 
{
	// 1. Verificación de seguridad: que el puntero de la lista sea válido
	if (lista_a_destruir == NULL) 
	{
		return; 
	}

	
	if (list_is_empty(lista_a_destruir)) // Si la lista ya estaba vacía
	{
		//solo destruye la estructura contenedora
		list_destroy(lista_a_destruir);
	} 
	else // Si la lista NO está vacía
	{
		// se destruye los elementos uno a uno y luego la estructura de la lista
		list_destroy_and_destroy_elements(lista_a_destruir, destruir_nodo_core);
	}
}
