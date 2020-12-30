/**
 * @brief: web_server.h
 * @desc: Contiene la definicion de las funciones para levantar el servidor web.
 * 
*/
#include <stdint.h>
#include "mongoose.h"

#ifndef WEB_H
#define WEB_H

void requests_handler(struct mg_connection *connection, int event, void *event_data, void *func_data);
void *web_server();

#endif