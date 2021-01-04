/** 
 * @brief: web_server.c
 * @desc: Conjunto de funciones que nos permitirán levantar el sevidor web, dicho servidor se encargará de 
 *        enviar las imágenes procesadas a través de HTTP hacia el cliente.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "mongoose.h"

static const char *s_listen_on = "http://localhost:8000";          // Dirección IP y puerto en el que correrá el servidor.
static const char *s_web_directory = ".";                           // Directorio dode se encuentran los recursos a ser servidos.

/**
 * @name: requests_handler
 * @desc: Se encarga de manejar las peticiones que llegan a este servidor. En este caso solo
 *        atenderemos peticiones HTTP, las peticiones que llegan a la ruta "/images/sobel"
 *        serán atendidas mandando la última imagen sobel procesada por el sistema. Cualquier
 *        otra petición de un recurso será atendida si dicho recurso existe en el directorio
 *        web especificado antes.
 * @parameters:
 *      connection : struct mg_connection *
 *          - Estructura que contiene toda la información pertinente a la conexión.
 *      event : int
 *          - Esta variable almacena el tipo de evento que ha ocurrido, este servidor prestará
 *            atención solo a los eventos HTTP.
 *      event_data : void *
 *          - Contiene la información del evento que ha ocurrido. Si se trata de una petición
 *            HTTP entonces contendrá la información del mensaje HTTP recibido.
 *      func_data : void *
 *          - Información personalizable, aquí pueden guardarse valores específicos de acuerdo
 *            a la aplicación. En este caso no la necesitamos.
*/
void requests_handler(struct mg_connection *connection, int event, void *event_data, void *func_data) 
{
    // CHECAMOS SI EL EVENTO QUE HA OCURRIDO ES UNA PETICIÓN HTTP
	if(event == MG_EV_HTTP_MSG) 
	{
        // COMO SABEMOS QUE SE TRATA DE UNA PETICIÓN HTTP CASTEAMOS LA INFORMACIÓN DEL EVENTO A UNA ESTRUCTURA
        // DE DATOS ESPECÍFICA HTTP.
		struct mg_http_message *http_msg = (struct mg_http_message *) event_data;

        // CHECAMOS SI LA URI QUE PIDE LA APP CLIENTE COINCIDE CON EL ENDPOINT QUE USAREMOS PARA MANDAR LAS IMÁGENES PROCESADAS
		if(mg_http_match_uri(http_msg, "/images/sobel")) 
		{
            // OBTENEMOS LOS DATOS DE LA IMAGEN Y SU LONGITUD PARA LUEGO MANDAR UNA RESPUESTA HTTP PERSONALIZADA, LA CUAL DEBE INDICAR
            // QUE SE ENCONTRÓ LA IMAGEN (STATUS 200), LA LONGITUD DE LA IMAGEN QUE VAMOS A MANDAR, SU TIPO MIME (image/bmp) Y UN
            // DETALLE MUY IMPORTANTE; PERMITIR EL ACCESO A ESTE RECURSO DESDE CUALQUIER APLICACIÓN, SI NO HACEMOS ESTO A PESAR DE QUE
            // SE ENVIÉ LA IMAGEN, EL BÚSCADOR WEB DEL CLIENTE NO DESPLEGARÁ LA IMAGEN.
			size_t img_size = mg_file_size("images/sobel.bmp");
			char *img_data = mg_file_read("images/sobel.bmp");
			mg_printf(connection, 
				"HTTP/1.1 200 OK\r\n"
				"Content-Length: %lu\r\n"
				"Access-Control-Allow-Origin: *\r\n"
				"Content-Type: image/bmp\r\n\r\n", (unsigned long) img_size);
			mg_send(connection, img_data, img_size);
			mg_send(connection, "\r\n", 2);
			free(img_data);
		} 
		else // ESTA FUNCIÓN SIRVE EL RECURSO SOLICITADO SI ESTE SE ENCUENTRA EN EL DIRECTORIO "s_web_directory"
			mg_http_serve_dir(connection, event_data, s_web_directory); 
	}
}

/**
 * @name: web_server
 * @desc: Se encarga de inicializar el servidor web.
*/
void *web_server()
{
	struct mg_mgr mgr;                            // Gestor de eventos

    // PONER EN 3 PARA HABILITAR DEBUG 
	mg_log_set("2");                        
    // INICIALIZANDO EL GESTOR DE EVENTOS
	mg_mgr_init(&mgr);                 
    // CREANDO EL SERVIDOR HTTP EL CUAL ESCUCHA EN LA DIRECCIÓN ESPECIFICADA (s_listen_on) Y QUE MANEJA LAS
    // PETICIONES A TRAVÉS DE LA FUNCIÓN "requests_handler"       
	mg_http_listen(&mgr, s_listen_on, requests_handler, NULL); 

    // EL SERVIDOR CORRE EN UN CICLO INFINITO BUSCANDO NUEVOS EVENTOS DE CONEXIÓN
	for(;;) 
		mg_mgr_poll(&mgr, 1000);
	mg_mgr_free(&mgr);

	pthread_exit(NULL);
}

