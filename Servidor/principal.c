/**
 * @brief: principal.c
 * @desc: Programa servidor que acepta conexiones de clientes, toma una imagen con las cámara de la Raspberry, 
 * 		  aplica un algoritmo de detección de bordes (Sobel) y envía dicha imagen ya procesada al cliente.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#include "defs.h"
#include "imagen.h"
#include "pdi.h"
#include "web_server.h"

unsigned char *imagenRGB, *imagenGray;				// Almacenarán la imagen en formato RGB y en escala de grises respectivamente
unsigned char *imagenGauss, *imagenSobel;			// Almacenarán la imagen después de haberle aplicado Gauss y Sobel respectivamente
uint32_t width = 0, height = 0; 					// Variables globales para que los hilos conozcan las dimensiones de la imagen
bmpInfoHeader info_imagen;							// Estructura de datos que contendrá la información de la imagen
int num_sample = 0;									// Contador del número de imagen muestra a tomar							

void runWebServer();
unsigned char *openImage();
void applyEdgeDetection();

int main(int argc, char const *argv[])
{
	// LANZAMOS EL SERVIDOR WEB, QUE CORRERÁ EN UN HILO
	runWebServer();

	// EJECUTAMOS EL ALGORITMO DE NAVEGACIÓN INDEFINIDAMENTE
	while(1)
	{
		// TOMANDO LA CAPTURA CON LA CÁMARA DE LA RASP, DICHA IMAGEN SE TOMA EN UNA RESOLUCIÓN DE 640x480
		// Y SE GUARDA EN FORMATO BMP.
		// if(system("raspistill -n -t 500 -e bmp -w 640 -h 480 -o foto.bmp") == -1)
		// {
		// 	perror("Ocurrio algun problema al tomar la captura");
		// 	exit(1);	
		// }
		
		// ABRIENDO LA IMAGEN RECIEN CAPTURADA
		imagenRGB = openImage();
		//displayInfoBMP(&info_imagen); 

		// RESERVANDO MEMORIA PARA LOS ARREGLOS QUE SE UTILIZAN POSTERIORMENTE (CUANDO SE USE RASP CON CÁMARA HACER ESTE PASO SOLO UNA VEZ)
		imagenGray = reservarMemoria(info_imagen.width, info_imagen.height);
		imagenGauss = reservarMemoria(info_imagen.width, info_imagen.height);
		imagenSobel = reservarMemoria(info_imagen.width, info_imagen.height);
		
		// TRANSFORMANDO LA IMAGEN A ESCALA DE GRISES
		RGBToGray(imagenRGB, imagenGray, info_imagen.width, info_imagen.height); 

		// APLICANDO GAUSS
		smoothGauss(imagenGray, imagenGauss, info_imagen.width, info_imagen.height);

		// APLICANDO SOBEL
		applyEdgeDetection();
		
		// GUARDANDO LA IMAGEN. PARA GUARDAR UNA IMAGEN SE NECESITA REGRESAR A FORMATO RGB.
		GrayToRGB(imagenRGB, imagenSobel, info_imagen.width, info_imagen.height);	
		saveBMP("images/sobel.bmp", &info_imagen, imagenRGB);
	
		free(imagenRGB);
		free(imagenGray);
		free(imagenSobel);
		free(imagenGauss);
		sleep(2);
	}

	printf("Concluimos la ejecución de la aplicacion Servidor\n");
	
	return 0;
}

/**
 * @name: runWebServer
 * @desc: Se encarga de lanzar el hilo que ejecutará el servidor web.
*/
void runWebServer()
{
	pthread_t web_thread_id;					// La ocupamos para guardar el ID del hilo que ejecuta el servidor web.

	// LANZAMOS EL SERVIDOR WEB EN UN HILO PARA QUE ATIENDA LAS PETICIONES DEL CLIENTE
	// SIN DETENER LA EJECUCIÓN DEL PROGRAMA PRINCIPAL
  	pthread_create(&web_thread_id, NULL, web_server, NULL);
	sleep(1);
}

/**
 * @name: applyEdgeDetection
 * @desc: Se encarga de lanzar los 4 hilos que aplicarán la función edgeDetection() 
*/
void applyEdgeDetection()
{
	register int num_thread;				    // Número de hilo a ejecutar.
	pthread_t thread_ids[NUM_THREADS];			// Arreglo de identificadores de los hilos.
	int nhs[NUM_THREADS];						// Arreglo de números de los hilos.
	int *num_hilo_finalizado;					// La ocupamos para capturar el número de hilo que haya finalizado. 

	// ASIGNANDO VALORES A LAS VARIABLES GLOBALES QUE TRABAJAN CON HILOS (CUANDO SE USE EN LA RASP CON CÁMARA HACER ESTE PASO UNA VEZ)
	width = info_imagen.width;		
	height = info_imagen.height;			
	
	// LIMPIANDO EL ARREGLO SOBEL (TAL VEZ NO NECESARIO CUANDO SE USE RASP CON CÁMARA)
	memset(imagenSobel, 0, width * height);

	// LLAMANDO HILOS PARA APLICAR DETECCIÓN DE BORDES
	for(num_thread = 0; num_thread < NUM_THREADS; num_thread++)
	{
		nhs[num_thread] = num_thread;
		pthread_create(&thread_ids[num_thread], NULL, edgeDetection, (void *) &nhs[num_thread]);
	}
	
	// ESPERANDO A QUE LOS HILOS TERMINEN SU EJECUCIÓN
	for(num_thread = 0; num_thread < NUM_THREADS; num_thread++)
	{
		pthread_join(thread_ids[num_thread], (void **) &num_hilo_finalizado);
		//printf("Hilo %d terminado\n", *num_hilo_finalizado);
	}
}

/**
 * @name: openImage
 * @desc: Se encarga de abrir la imagen respectiva de muestra, pues no contamos con la cámara aún.
 * 		  Los datos y cabecera de la imagen se almacenan en las variables globales asignadas para ello.
*/
unsigned char *openImage()
{
	char img_dir_path[] = "images/";		// Directorio en donde se encuentran las imágenes de muestra.
	char img_filename[20];					// Alamacena el nombre de la imagen de muestra i.e. "sample3.bmp"
	char img_abs_filename[100];				// Almacena la ruta completa donde se encuentra la imagen muestra.
	
	printf("Abriendo imagen...\n");
	// ARMANDO LA RUTA COMPLETA DE LA IMG MUESTRA A ABRIR
	sprintf(img_filename, "sample%d.bmp", (num_sample % 4));
	strcpy(img_abs_filename, img_dir_path);
	strcat(img_abs_filename, img_filename);
	
	num_sample++;
	printf("%s\n", img_abs_filename);
	
	// ABRIMOS LA IMAGEN. OBTENEMOS LOS DATOS Y LA INFORMACIÓN DE CABECERA
	imagenRGB = openBMP(img_abs_filename, &info_imagen);

	return imagenRGB;
}