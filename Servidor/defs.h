/**
 * @brief: defs.h
 * @desc: Archivo de definiciones
*/

#ifndef DEFS_H
#define DEFS_H

#define GAUSS_MASK      3       // Dimensión del filtro(matriz) de Gauss, en este caso de 3x3
#define SOBEL_MASK      3       // Dimensión del filtro(matriz) de Sobel, en este caso de 3x3
#define NUM_THREADS     4  		// Número de hilos para procesar la imagen cuando se aplica Sobel
#define IMG_WIDTH		640
#define IMG_HEIGHT		480
#define IMG_CHANNELS	3
#define IMG_SIZE	    IMG_WIDTH * IMG_HEIGHT * IMG_CHANNELS	// Tamaño de la imagen


#endif

