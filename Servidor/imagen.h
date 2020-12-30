/**
 * @brief: imagen.h
 * @desc: Contiene la definicion de las funciones y estructuras necesarias
 * 		  para el manejo de archivos BMP.
*/
#include <stdint.h>
#include <stdio.h>

#ifndef IMAGEN_H
#define IMAGEN_H

typedef struct bmpFileHeader
{
	// 2 bytes de identificación 
	uint32_t size;              // Tamaño del archivo
	uint16_t resv1;         	// Reservado 
	uint16_t resv2;       		// Reservado 
	uint32_t offset;      		// Offset hasta hasta los datos de imagen
} bmpFileHeader;

typedef struct bmpInfoHeader
{
	uint32_t headersize;      	// Tamaño de la cabecera 
	uint32_t width;           	// Ancho 
	uint32_t height;      		// Alto
	uint16_t planes;          	// Planos de color (Siempre 1)
	uint16_t bpp;             	// Bits por pixel
	uint32_t compress;        	// Compresión
	uint32_t imgsize;     		// tamaño de los datos de imagen
	uint32_t bpmx;        		// Resolución X en bits por metro
	uint32_t bpmy;        		// Resolución Y en bits por metro 
	uint32_t colors;      		// colors used en la paleta 
uint32_t imxtcolors;      		// Colores importantes. 0 si son todos
} bmpInfoHeader;

unsigned char *openBMP(char *filename, bmpInfoHeader *bInfoHeader);
void saveBMP(char *filename, bmpInfoHeader *info, unsigned char *imgdata);
void displayInfoBMP(bmpInfoHeader *info);

#endif
