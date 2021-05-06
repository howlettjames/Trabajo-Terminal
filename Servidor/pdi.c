/** 
 * @brief: pdi.c
 * @desc: Conjunto de funciones que nos permitirán aplicar la batería de transformaciones propuesta
 *        sobre la imagen, estas son; transformación a escala de grises, limpieza de ruido utilizando Gauss,
 * 		  y detección de bordes utilizando Sobel.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>

#include "pdi.h"
#include "defs.h"

extern unsigned char *imagenGauss, *imagenSobel;		// Variables globales en donde los hilos guardarán el resultado de sus operaciones


/**
 * @name: sumar
 * @desc: Aplica la union entre la imagen segmentada y la imagen Sobel con contornos detectados
 * @parameters:
 *      imagenSobel : unsigned char *
 *          - Es la imagen con los contornos detectados
 *		  imagenSegmentada : unsigned char *
 *          - Es la imagen segmentada
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
 *      threshold : int
 *          - Es el umbral a considerar para aplicar la segmentacion
*/
void sumar(unsigned char *imagenSobel, unsigned char *imagenSegmentada, unsigned char *imagenUnion, uint32_t _width, uint32_t _height)
{
    register int indice;
    
    memset(imagenUnion, 255, _width * _height);   // Limpiamos la estructura
    
    for (indice = 0; indice < (_width * _height); indice++) {
    		imagenUnion[indice] = (~imagenSobel[indice]) & (imagenSegmentada[indice]);
    		//if (imagenUnion[indice] == 0) {imagenUnion[indice] = 255;}
    		//else {imagenUnion[indice] = 0;}		    
    }   
}

/**
 * @name: segmentar
 * @desc: Aplica una segmentacion utilizando un limite de umbralizacion
 * @parameters:
 *      imagenGray : unsigned char *
 *          - Es la imagen en escala de grises
 *		  imagenSegmentada : unsigned char *
 *          - Es la imagen segmentada
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
 *      threshold : int
 *          - Es el umbral a considerar para aplicar la segmentacion
*/
void segmentar(unsigned char *imagenGray, unsigned char *imagenSegmentada, uint32_t _width, uint32_t _height, int threshold)
{
    register int indiceGray;
    
    memset(imagenSegmentada, 255, _width * _height);   // Limpiamos la estructura
    
    if (threshold > 45)
    {
        for (indiceGray = 0; indiceGray < (_width * _height); indiceGray++)
        {
				if (imagenGray[indiceGray] < threshold) { imagenSegmentada[indiceGray] = 0; }
				else { imagenSegmentada[indiceGray] = 255; }                 
        }
    }
    else
    {
        for (indiceGray = 0; indiceGray < (_width * _height); indiceGray++)
        {
				if (imagenGray[indiceGray] < 127) { imagenSegmentada[indiceGray] = 0; }
				else { imagenSegmentada[indiceGray] = 255; }                 
        }
    }
}

/**
 * @name: edgeDetection
 * @desc: Aplica un filtro para detección de bordes utilizando el operador 
 *        de Sobel, el filtro es aplicado utilizando 4 hilos (HPC) en donde cada
 *        hilo aplica el filtro sobre la cuarta parte de la imagen que le
 *        corresponde.
 * @parameters:
 *      nh : void *
 *          - Número de hilo, se utiliza para calcular el bloque de la imagen que
 * 			  le corresponde a este hilo procesar.
 *      imagenGauss : unsigned char *
 *          - Es la imagen resultante de aplicar el filtro Gaussiano. Sobre dicha 
 * 			  imagen se aplicará el filtro Sobel.
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
*/
void *edgeDetection(void *nh)
{
	// CALCULANDO EL BLOQUE QUE LE TOCA PROCESAR A ESTE HILO
	int core = *(int *)nh;
	int elemsBloque = (IMG_WIDTH * IMG_HEIGHT) / NUM_THREADS;
	int inicioBloque = (core * elemsBloque);
	int finBloque = (inicioBloque + elemsBloque) / IMG_WIDTH;
	inicioBloque = inicioBloque / IMG_WIDTH;	

	// VARIABLES PARA TRABAJAR CON SOBEL
	register int x, y, xb, yb;								// Índices e índices offset (b)
	int convolucion1, convolucion2, indice, resultado;
	int gradiente_fila[9] = {1, 0, -1, 2, 0, -2, 1, 0, -1}; // Gradientes de Sobel
	int gradiente_col[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};	// Gradientes de Sobel

	if(core == 3) // Cuando se trata del último bloque se resta la dimensión para no calcular fuera del límite de la imagen
		finBloque = finBloque - SOBEL_MASK;

	// RECORREMOS LA IMAGEN Y APLICAMOS SOBEL DE ACUERDO AL BLOQUE QUE LE TOCÓ A ESTE HILO
	for(y = inicioBloque; y < finBloque; y++)
		for(x = 0; x < IMG_WIDTH - SOBEL_MASK; x++)
		{
			// HACEMOS LA CONVOLUCION DEL FILTRO CON LA IMAGEN
			resultado = 0;
			for (yb = 0; yb < SOBEL_MASK; yb++)
				for (xb = 0; xb < SOBEL_MASK; xb++)
				{
					indice = (y + yb) * IMG_WIDTH + (x + xb);
					convolucion1 += gradiente_fila[yb * SOBEL_MASK + xb] * imagenGauss[indice];
					convolucion2 += gradiente_col[yb * SOBEL_MASK + xb] * imagenGauss[indice];
				}
			
			// GUARDAMOS EL RESULTADO. SE DIVIDE ENTRE DOS UTILIZANDO CORRIMIENTO PARA MAYOR RAPIDEZ.
			convolucion1 = convolucion1 >> 2;
			convolucion2 = convolucion2 >> 2;
			resultado = (int) sqrt(convolucion1 * convolucion1 + convolucion2 * convolucion2);

			// SE CALCULA EL ÍNDICE EN EL CUAL SE VA A GUARDAR EL RESULTADO Y SE GUARDA.
			indice = (y + (SOBEL_MASK >> 1)) * IMG_WIDTH + (x + (SOBEL_MASK >> 1));
			imagenSobel[indice] = resultado;
		}

	pthread_exit(nh);
}

/**
 * @name: smoothGauss
 * @desc: Aplica un filtro Gaussiano de 3x3 a una imagen en escala
 *        de grises con el fin de eliminar ruido y difuminarla.
 * @parameters:
 *      imagenGray : unsigned char *
 *          - Imagen en escala de grises
 *      imagenGauss : unsigned char *
 *          - Imagen resultante del filtro Gaussiano
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
*/
void smoothGauss(unsigned char *imagenGray, unsigned char *imagenGauss, uint32_t _width, uint32_t _height)
{
    register int x, y, xm, ym;
    int indicem, indicei, conv;
		
		int mascara[GAUSS_MASK*GAUSS_MASK] = {  1, 2, 1,
                                              2, 4, 2,
                                              1, 2, 1 };
                                            
		/*int mascara[GAUSS_MASK*GAUSS_MASK] = {  1,  4, 1,
                                              4, 12, 4,
                                              1,  4, 1 };*/
                                              
		/*int mascara[GAUSS_MASK*GAUSS_MASK] = {  1,  4,  6,  4, 1,
							4, 16, 24, 16, 4,
							6, 24, 36, 24, 6,
							4, 16, 24, 16, 4, 
							1,  4,  6,  4, 1 };*/                          
	
    memset(imagenGauss, 255, _width * _height);   // Limpiamos la estructura
    for (y = 0; y <= (_height - GAUSS_MASK); y++)
		for (x = 0; x <= (_width - GAUSS_MASK); x++)
		{
			indicem = 0;
			conv = 0;
			for (ym = 0; ym < GAUSS_MASK; ym++)
				for (xm = 0; xm < GAUSS_MASK; xm++)
				{
					indicei = (y + ym) * _width + (x + xm);
					conv += imagenGray[indicei] * mascara[indicem++];
				}
			conv >>= 4; // conv = conv / 16	G3
			//conv >>= 5; // conv = conv / 32		Debil
			//conv /= 246;				G5
			indicei = (y + 1) * _width + (x + 1);
			imagenGauss[indicei] = conv;
		}
}

/**
 * @name: RGBToGray
 * @desc: Realiza la conversión de una imagen en el dominio RGB de tres
 *        canales a escala de grises, con un solo canal de color, normalizando
 *        la intensidad de cada uno de los canales y finalmente lo almacena
 *        en la estructura imagenGray
 * @parameters:
 *      imagenRGB : unsigned char *
 *          - Representación de la imagen en el espacio RGB
 *      imagenGray : unsigned char *
 *          - Es la imagen resultante de la conversion a escala de grises
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
*/
void RGBToGray(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t _width, uint32_t _height)
{
    unsigned char nivelGris;
    register int indiceGray, indiceRGB;
	 
    for (indiceGray = 0, indiceRGB = 0; indiceGray < (_width * _height); indiceGray++, indiceRGB += 3)
    {
        nivelGris = ((21 * imagenRGB[indiceRGB]) + (72 * imagenRGB[indiceRGB + 1]) + (7 * imagenRGB[indiceRGB + 2])) / 100;
        imagenGray[indiceGray] = nivelGris;
    }
    
}

/**
 * @name: ajusteBrilloImagen
 * @desc: Modifica el brillo o contraste de la imagen a partir de un valor.
 *        Si el valor del desplazamiento es negativo, se oscurece la imagen
 *        pero si es positivo, el brillo se aumenta. Es lo mismo que desplazar
 *        el histograma de la imagen a la izquierda o derecha 
 * @parameters:
 *      imagenGray : unsigned char *
 *          - Es la imagen en escala de grises
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
 *      desplazamiento : unsigned char
 *          - Es el valor de desplazamiento del histograma, el cual va de 0 a 255
*/
void ajusteBrilloImagen(unsigned char * imagenGray, uint32_t _width, uint32_t _height, int desplazamiento)
{
    register int indiceGray;
    int pixel;

    if (desplazamiento > 0) 
    {
        for (indiceGray = 0; indiceGray < (_width * _height); indiceGray++)
        {
            pixel = imagenGray[indiceGray] + desplazamiento;
            imagenGray[indiceGray] = (pixel > 255) ? 255 : (unsigned char)pixel;
        }
    }
    else
    {
        for (indiceGray = 0; indiceGray < (_width * _height); indiceGray++)
        {
            pixel = imagenGray[indiceGray] + desplazamiento;
            imagenGray[indiceGray] = (pixel < 0) ? 0 : (unsigned char)pixel;
        }
    }
}

/**
 * @name: GrayToRGB
 * @desc: Convierte una imagen en escala de grises a una RGB.
 *        Sin embargo, no recupera el color original. Simplemente
 *        es para poder almacenar la imagen y visualizar en el S.O
 * @parameters:
 *      imagenRGB : unsigned char *
 *          - Representación de la imagen en el espacio RGB
 *      imagenGray : unsigned char *
 *          - Imagen en escala de grises
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
*/
void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t _width, uint32_t _height)
{
    register int indiceGray, indiceRGB;

    for (indiceGray = 0, indiceRGB = 0; indiceGray < (_width * _height); indiceGray++, indiceRGB += 3)
    {
        imagenRGB[indiceRGB] = imagenGray[indiceGray];      // Componente R
        imagenRGB[indiceRGB + 1] = imagenGray[indiceGray];  // Componente G
        imagenRGB[indiceRGB + 2] = imagenGray[indiceGray];  // Componente B
    }
}

/**
 * @name: reservarMemoria
 * @desc: Crea un espacio de memoria para una imagen RGB
 * @parameters:
 *      width : uint32_t
 *          - Ancho de la imagen RGB
 *      height : uint32_t
 *          - Altura de la imagen RGB
 * @returns:
 *      imagen : unsigned char *
 *          - Representación de la imagen
*/
unsigned char * reservarMemoria(uint32_t _width, uint32_t _height)
{
    unsigned char *imagen;
    imagen = (unsigned char *)malloc(_width * _height * sizeof(unsigned char));
    
    if(!imagen)
    {
	    perror("Error al asignar memoria a la imagen");
	    exit(EXIT_FAILURE);
    }
    return imagen;
}
