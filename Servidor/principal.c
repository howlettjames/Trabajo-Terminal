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

unsigned char *imagenRGB, *imagenGray;				// Almacenarán la imagen en formato RGB y en escala de grises respectivamente
unsigned char *imagenGauss, *imagenSobel;			// Almacenarán la imagen después de haberle aplicado Gauss y Sobel respectivamente
bmpInfoHeader info_imagen;							// Estructura de datos que contendrá la información de la imagen
int num_sample = 0;									// Contador del número de imagen muestra a tomar							

void applyEdgeDetection();

int main(int argc, char const *argv[])
{
	// RESERVANDO MEMORIA PARA LOS ARREGLOS QUE SE UTILIZAN POSTERIORMENTE (CUANDO SE USE RASP CON CÁMARA HACER ESTE PASO SOLO UNA VEZ)
	imagenGray = reservarMemoria(IMG_WIDTH, IMG_HEIGHT);
	imagenGauss = reservarMemoria(IMG_WIDTH, IMG_HEIGHT);
	imagenSobel = reservarMemoria(IMG_WIDTH, IMG_HEIGHT);

	// EJECUTAMOS EL ALGORITMO DE NAVEGACIÓN INDEFINIDAMENTE
	while(1)
	{
		// TOMANDO LA CAPTURA CON LA CÁMARA DE LA RASP, DICHA IMAGEN SE TOMA EN UNA RESOLUCIÓN DE 640x480
		// Y SE GUARDA EN FORMATO BMP.
		if(system("raspistill -n -t 500 -e bmp -w 640 -h 480 -o images/sample.bmp") == -1)
		{
		 	perror("Ocurrio algun problema al tomar la captura");
		 	exit(1);	
		}
		
		// ABRIENDO LA IMAGEN RECIEN CAPTURADA
		imagenRGB = openBMP("images/sample.bmp", &info_imagen);
		//displayInfoBMP(&info_imagen);		
	
		// TRANSFORMANDO LA IMAGEN A ESCALA DE GRISES		
		RGBToGray(imagenRGB, imagenGray, info_imagen.width, info_imagen.height); 
		GrayToRGB(imagenRGB, imagenGray, info_imagen.width, info_imagen.height);	
		saveBMP("images/gray.bmp", &info_imagen, imagenRGB);
		
		// APLICANDO GAUSS
		smoothGauss(imagenGray, imagenGauss, info_imagen.width, info_imagen.height);
		GrayToRGB(imagenRGB, imagenGauss, info_imagen.width, info_imagen.height);	
		saveBMP("images/smooth.bmp", &info_imagen, imagenRGB);

		// APLICANDO SOBEL
		applyEdgeDetection();

		// GUARDANDO LA IMAGEN. PARA GUARDAR UNA IMAGEN SE NECESITA REGRESAR A FORMATO RGB.
		GrayToRGB(imagenRGB, imagenSobel, info_imagen.width, info_imagen.height);	
		saveBMP("images/sobel.bmp", &info_imagen, imagenRGB);
	
		free(imagenRGB);
		//sleep(1);	
	}

	free(imagenGray);
	free(imagenSobel);
	free(imagenGauss);
	printf("Concluimos la ejecución de la aplicacion Servidor\n");
	
	return 0;
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

	// LIMPIANDO EL ARREGLO SOBEL (TAL VEZ NO NECESARIO CUANDO SE USE RASP CON CÁMARA)
	memset(imagenSobel, 0, IMG_WIDTH * IMG_HEIGHT);

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
