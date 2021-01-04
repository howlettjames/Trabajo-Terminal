/**
 * @brief: defs.h
 * @desc: Archivo de definiciones
*/

#ifndef DEFS_H
#define DEFS_H

#define GAUSS_MASK      3       // Dimensión del filtro(matriz) de Gauss, en este caso de 3x3
#define SOBEL_MASK      3       // Dimensión del filtro(matriz) de Sobel, en este caso de 3x3
#define NUM_THREADS     4  		// Número de hilos para procesar la imagen cuando se aplica Sobel
#define WIDTH_IMG		640
#define HEIGHT_IMG		480
#define CHANNELS_IMG	3
#define IMG_SIZE	    WIDTH_IMG*HEIGHT_IMG*CHANNELS_IMG	// Tamaño de la imagen


#endif

