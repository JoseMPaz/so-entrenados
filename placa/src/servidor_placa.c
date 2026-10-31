#include "servidor_placa.h"

//Gestiona las conexiones a la placa
void * servidor_placa (void * socket)
{
	int socket_escucha = *(int *)socket;
	int socket_aceptado;
    
	// Declaramos las variables una sola vez al principio para evitar errores de redefinición
	int * socket_de_atencion;
	t_paquete * paquete;
	pthread_t hilo_de_atencion_a_planificador;
	pthread_t hilo_de_atencion_a_core;
	t_operacion operacion;
	t_list * carga_util = NULL;
	uint32_t id_core;
	char * ultimo_caracter;
				
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST)) 
	{
		socket_aceptado = accept (socket_escucha, NULL, NULL);//Bloqueante hasta que el Planificador o algun Core se conecte
    	
    	if (socket_aceptado >= 0)
   		{
   			operacion = recibir_operacion (socket_aceptado);
   			
   			switch (operacion)
   			{
   				// **************************************************
   				case SOLICITUD_CONEXION_PLANIF_TO_PLACA://Handshake Planificador -> Placa
   				
   					if (cantidad_de_kernel == 0) //Cuando aun no se conecto el kenel - solo debe existir 1 kernel
   					{
   						// LOG 1 1/2: Obligatorio
   						log_info (historial, "%s%s",  "Conexion recibida: ", "##Modulo: Planificador");
   						//Contesta el handshake al planificador que se conecto
   						paquete = crear_paquete (RESPUESTA_CONEXION_PLACA_TO_PLANIF_OK);
   						enviar_paquete (paquete, socket_aceptado);
   						destruir_paquete (paquete);
   						//Creamos un hilo de atencion para que procese las peticiones del planificador
   						socket_de_atencion = (int *) malloc (sizeof(int)); //Se libera en la funcion de atencion
   						*socket_de_atencion = socket_aceptado;
   						pthread_create(&hilo_de_atencion_a_planificador, NULL, atender_peticiones_del_planificador, (void *) socket_de_atencion);
   						pthread_detach (hilo_de_atencion_a_planificador);
   						cantidad_de_kernel++;
   					}
   					else
   					{
   						fprintf (stderr, "El sistema solo acepta 1 planificador\n");
   						exit (EXIT_FAILURE);
   					}
   					break;
   				
   				// **************************************************
   				case SOLICITUD_CONEXION_CORE_TO_PLACA://Handshake cada Core -> Placa
   					
   					carga_util = recibir_carga_util (socket_aceptado);
   					id_core = (uint32_t) strtol (list_get (carga_util, ID_CORE) ,&ultimo_caracter, BASE_DECIMAL);
   					if (*ultimo_caracter != '\0')
   					{
   						fprintf (stderr, "%s\n", "El id del core debe ser numerico");
   						exit (EXIT_FAILURE);
   					}
   					list_destroy_and_destroy_elements (carga_util, free);
   					// LOG 1 2/2: Obligatorio
   					log_info (historial, "%s%s%s%u",  "Conexion recibida: ", "##Modulo: Core, ", "Id: ", id_core);
   					//Contesta el handshake al core que se conecto
   					paquete = crear_paquete (RESPUESTA_CONEXION_PLACA_TO_CORE_OK);
   					enviar_paquete (paquete, socket_aceptado);
   					destruir_paquete (paquete);
					
   					//Creamos un hilo de atencion para que procese las peticiones del core conectado
   					socket_de_atencion = (int *) malloc (sizeof(int));//Se libera en la funcion de atencion
   					*socket_de_atencion = socket_aceptado;
   					pthread_create(&hilo_de_atencion_a_core, NULL, atender_peticiones_de_un_core, (void *) socket_de_atencion);
   					pthread_detach (hilo_de_atencion_a_core);
   					break;
   				
   				// **************************************************
   				default://Para handshake invalidos
   					paquete = crear_paquete (RESPUESTA_CONEXION_FALLIDA);
   					enviar_paquete (paquete, socket_aceptado);
   					destruir_paquete (paquete);
                    close(socket_aceptado); // Evitamos leaks de sockets
   					break;
   			}   			
   		}    	
	}

    printf("Hilo del servidor placa cerrándose de forma limpia...\n");
    pthread_exit(NULL);
}

//Atiende las solicitudes del planificador
void * atender_peticiones_del_planificador (void * socket)
{
	t_operacion operacion;
	t_paquete * paquete;
	t_list * carga_util = NULL;
	int socket_planificador = *(int *)socket;
	int jid;
	char * path_instrucciones = NULL;
	t_tabla_de_pagina * una_tabla_de_paginas;
	
	
	if (socket != NULL)
		free (socket);
	
	/* La peticiones que el planificador puede pedir a placa son:
	PLANIF_TO_PLACA_CREATE_JOB (ok)
    PLANIF_TO_PLACA_ALLOC_SPACE
    PLANIF_TO_PLACA_FREE_SPACE
    PLANIF_TO_PLACA_LOAD_PAGE
    PLANIF_TO_PLACA_READ_DATA
    PLANIF_TO_PLACA_WRITE_DATA
    PLANIF_TO_PLACA_UNLOCK_PAGES
    PLANIF_TO_PLACA_EXIT_JOB
	*/
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST))
	{
		int operacion_recibida = recibir_operacion(socket_planificador);

		// Si es -1, sabemos con total certeza que el Planificador se desconectó
		if (operacion_recibida == -1) 
		{
			fprintf(stderr, "%s\n", "[Placa]: El Planificador se ha desconectado de forma limpia o abrupta.");
			break; // Rompe el while, sale del bucle y va directo al close(socket) final
		}

		operacion = (t_operacion)operacion_recibida;
		
		switch (operacion)
   		{
   			case PLANIF_TO_PLACA_CREATE_JOB:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				/*recibir carga util: jid - path_instrucciones y cargarlo en lista instrucciones_de_jobs*/
   				carga_util = recibir_carga_util (socket_planificador);
   				
   				jid = (int) strtol (list_get (carga_util, JID), NULL, BASE_DECIMAL);
   				path_instrucciones = list_get (carga_util, PATH_INSTRUCCIONES);
   				list_destroy_and_destroy_elements (carga_util, free);
   				
   				list_add (instrucciones_de_jobs, crear_codigo_proceso (jid, path_instrucciones) );//Requiere mutex
   								
				//list_iterate(instrucciones_de_jobs, imprimir_instrucciones);//Sera borrado
				
				una_tabla_de_paginas = crear_tabla_paginas_proceso (jid);
				
				list_add (tablas_de_paginas_de_jobs, una_tabla_de_paginas);
								
				// LOG 2: Obligatorio
				log_info (historial, "Creación de Job: “## JID: %u - Job Creado”", jid); 
   				
   				paquete = crear_paquete (OPERACION_OK);
				enviar_paquete (paquete, socket_planificador);
				destruir_paquete (paquete);
   				break;
   			
   			case PLANIF_TO_PLACA_ALLOC_SPACE:
   				usleep(retardo_memoria * 1000); 
   				
   				// 1. Recibir los datos del Planificador
   				carga_util = recibir_carga_util(socket_planificador);
   				
   				jid = (int) strtol(list_get(carga_util, JID), NULL, BASE_DECIMAL);
   				int bytes_solicitados = (int) strtol(list_get(carga_util, BYTES), NULL, BASE_DECIMAL);
   				
   				list_destroy_and_destroy_elements(carga_util, free);

   				// 2. Calcular cuántas páginas virtuales se requieren
   				int paginas_necesarias = (int) ceil((double)bytes_solicitados / tam_pagina);

   				// 3. Buscar la tabla de páginas del proceso
   				jid_a_buscar = jid; 
   				
   				t_tabla_de_pagina * tabla_proceso = list_find(tablas_de_paginas_de_jobs, buscar_por_jid);
   				
   				if (tabla_proceso == NULL) 
   				{
   					paquete = crear_paquete(OPERACION_ERROR);
					enviar_paquete(paquete, socket_planificador);
					destruir_paquete(paquete);
   					break;
   				}

   				// 4. VALIDAR espacio en el Offload general
   				int bloques_libres_offload = cantidad_bloques_totales - contar_bloques_ocupados_offload();
   				
   				if (bloques_libres_offload < paginas_necesarias) 
   				{
   					log_error(historial, "Error Alloc Space: Offload sin espacio suficiente para JID: %u", jid);
   					paquete = crear_paquete(OPERACION_ERROR);
					enviar_paquete(paquete, socket_planificador);
					destruir_paquete(paquete);
   					break;
   				}

   				// 5. Calcular la Dirección Lógica Inicial
   				uint32_t pagina_inicial_nueva = list_size(tabla_proceso->tabla);
   				
   				uint32_t dir_logica_resultado = pagina_inicial_nueva * tam_pagina;

   				// 🚀 6. Reservar bloques en el Offload y crear entradas dinámicas según el Algoritmo
   				for (int i = 0; i < paginas_necesarias; i++) 
   				{
   					// Reservamos el bloque en tu bitmap global
   					int nro_bloque_offload_asignado = obtener_bloque_libre_offload (); 
   					
   					// Llamamos a tu función polimórfica que genera el struct correcto (LRU o CLOCK-M)
   					void * nueva_pagina = crear_entrada_pagina(nro_bloque_offload_asignado);
   					
   					// Lo agregamos a la t_list 'tabla' del proceso
   					list_add(tabla_proceso->tabla, nueva_pagina);
   				}

				// LOG 4 1/2 Obligatorio
				log_info(historial, "Asignacion de espacio: ## JID: %u - Asignación - Dirección Lógica: %u - Tamaño: %d", jid, dir_logica_resultado, bytes_solicitados); 
   				
   				// 7. Responder OPERACION_OK con la dirección lógica
   				paquete = crear_paquete(OPERACION_OK);
   				agregar_a_paquete(paquete, (void *)&dir_logica_resultado, sizeof(uint32_t));
				enviar_paquete(paquete, socket_planificador);
				destruir_paquete(paquete);
   				break;
   			
   			case PLANIF_TO_PLACA_FREE_SPACE:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			case PLANIF_TO_PLACA_LOAD_PAGE:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			case PLANIF_TO_PLACA_READ_DATA:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			case PLANIF_TO_PLACA_WRITE_DATA:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			case PLANIF_TO_PLACA_UNLOCK_PAGES:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			case PLANIF_TO_PLACA_EXIT_JOB:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   				
   			default:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/);
   				paquete = crear_paquete (OPERACION_ERROR);
				enviar_paquete (paquete, socket_planificador);
				destruir_paquete (paquete);
   		}
	}
	
	close (socket_planificador);//No debe llegr aca ya que es un servidor
	
	pthread_exit(NULL);
}

//Atiende las solicitudes de cada core
void * atender_peticiones_de_un_core (void * socket)
{
	t_operacion operacion;
	t_paquete * paquete;
	t_list * carga_util = NULL;
	int program_counter;
	int jid; 
	char * instruccion;
	t_codigo_proceso * nodo = NULL;
	int socket_core = *(int *)socket;
	
	if (socket != NULL)
		free (socket);
		
	/* La peticiones que el planificador puede pedir a placa son:
	CORE_TO_PLACA_FETCH_INSTRUCTION,(ok)
    CORE_TO_PLACA_GET_FRAME,
    CORE_TO_PLACA_READ_DATA,
    CORE_TO_PLACA_WRITE_DATA,
    CORE_TO_PLACA_UNLOCK_PAGES,
	*/
	while (!__atomic_load_n(&salir, __ATOMIC_SEQ_CST))
	{
		int operacion_recibida = recibir_operacion(socket_core);

		if (operacion_recibida == -1) 
		{
			printf("[Placa]: El Core se ha desconectado de forma limpia o abrupta.\n");
			break; 
		}

		operacion = (t_operacion)operacion_recibida;
		
		switch (operacion)
   		{
   			case CORE_TO_PLACA_FETCH_INSTRUCTION:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				
   				//Debe recibir el jid y el pc como operandos
   				carga_util = recibir_carga_util (socket_core);
   				
   				//Carga de operando
   				jid = (int) strtol (list_get (carga_util, JID), NULL, BASE_DECIMAL);
   				program_counter = (int) strtol (list_get (carga_util, PROGRAM_COUNTER), NULL, BASE_DECIMAL);
   				
   				// Se setea la variable global jid_a_buscar para que el clousure buscar_por_jid lo pueda usar al buscar
   				jid_a_buscar = jid;
   				//Busca las instrucciones a partir del jid en la lista de instrucciones de jobs, se carga cuando se crear el job
   				nodo = (t_codigo_proceso *) list_find (instrucciones_de_jobs, buscar_por_jid);
   				/*
   					nodo ->  struct{ uint32_t pid; 	t_list * instrucciones;}
   				*/
   				if (nodo!= NULL)//Lo encontro
   				{
   					
   					instruccion = list_get (nodo->instrucciones/*Es una lista*/, program_counter/*numero de instruccion*/);
   					paquete = crear_paquete (OPERACION_OK);
   					// Mandamos la instruccion sumando + 1 a su longitud para incluir el '\0'
   					agregar_a_paquete (paquete, (void *) instruccion, strlen (instruccion) + 1);
   				}
   				else
   				{
   					printf("[Placa]: Error - No se encontró el Job %d en la memoria de instrucciones.\n", jid);
   					paquete = crear_paquete (OPERACION_ERROR);
   				}
   		
				enviar_paquete (paquete, socket_core);
				
				// LOG 3: Obligatorio
				log_info (historial, "Obtener instrucción: ## JID: %u - Obtener instrucción: %d - Instrucción: %s", jid, program_counter, instruccion);
				
				list_destroy_and_destroy_elements (carga_util, free);
				destruir_paquete (paquete);
   				break;
   			case CORE_TO_PLACA_GET_FRAME:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   				
   			case CORE_TO_PLACA_READ_DATA:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   				
   			case CORE_TO_PLACA_WRITE_DATA:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   			
   			case CORE_TO_PLACA_UNLOCK_PAGES:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/); 
   				break;
   				
   					   				
   			default:
   				usleep(retardo_memoria/*esta en ms*/ * 1000/*Para pasarlo a us*/);
   				paquete = crear_paquete (OPERACION_ERROR);
				enviar_paquete (paquete, socket_core);
				destruir_paquete (paquete);
				break;
   		}
	}
	
	close (socket_core);
	pthread_exit(NULL);
}

bool buscar_por_jid (void * elemento) 
{
    t_codigo_proceso * proceso = (t_codigo_proceso*) elemento;
    return proceso->jid == jid_a_buscar;
}
