/** 
 * @brief: imagen.c
 * @desc: Conjunto de 3 funciones que nos permiten manipular archivos de imagen de tipo BMP.
 *        Dichas funciones nos ayudan a: abrir archivos BMP, guardar una imagen en formato BMP y
 *        desplegar la información de cabecera un archivo BMP.
*/
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "imagen.h"
#include "defs.h"

/**
 * @name: openBMP
 * @desc: Se encarga de extraer la infomación de la imagen que lleva por nombre "filename".
 *        Los datos que extrae son; los bytes que representan a la imagen en un arreglo llamado "imgdata"
 *        y la cabecera de información de la imagen que es almacenada en el struct "bInfoHeader".     
 * @parameters:
 *      filename : char *
 *          - Nombre de la imagen a abrir.
 *      bInfoHeader : bmpInfoHeader *
 *          - Struct que almacena la información pertinente a la imagen, a destacar el ancho y alto en 
 * 			  píxeles.
*/
unsigned char *openBMP(char *filename, bmpInfoHeader *bInfoHeader)
{
  	FILE *f;						// Apuntador a la imagen
  	bmpFileHeader header;     		// Cabecera de la imagen
  	unsigned char *imgdata;   		// Datos de la imagen
 	uint16_t type;            		// 2 bytes identificativos del tipo de archivo

	// Abriendo archivo BMP
	f = fopen(filename, "r");
	if(f  == NULL)
	{
		perror("Error al abrir el archivo en lectura");
		exit(EXIT_FAILURE);
	}

  	// Leemos los dos primeros bytes que son los que nos indican cual es el formato de la imagen
	// Comprobamos que el formato sea BMP por su código 0x4D42
  	fread(&type, sizeof(uint16_t), 1, f);
  	if(type != 0x4D42)        
	{
	    printf("Error en el formato de imagen, debe ser BMP de 24 bits");
  	    fclose(f);
  		return NULL;
	}

  	// Leemos la cabecera de fichero completa
  	fread(&header, sizeof(bmpFileHeader), 1, f);

  	// Leemos la cabecera de información completa
  	fread(bInfoHeader, sizeof(bmpInfoHeader), 1, f);

	// Asignamos el tamaño de la imagen manualmente, dado que el comando raspistill no lo hace.
	bInfoHeader->imgsize = IMG_SIZE;

  	// Reservamos memoria para la imagen, ¿cuánta? Tanto como indique imgsize 
  	imgdata = (unsigned char *) malloc(bInfoHeader->imgsize);
	if(imgdata == NULL)
	{
		perror("Error al asignar memoria");
		exit(EXIT_FAILURE);
	}

  	// Nos situamos en el sitio donde empiezan los datos de imagen, nos lo indica el offset de la cabecera de fichero 
  	fseek(f, header.offset, SEEK_SET);
  	
	// Leemos los datos de imagen, tantos bytes como imgsize 
  	fread(imgdata, bInfoHeader->imgsize, 1, f);
  	fclose(f);

  	return imgdata;
}

/**
 * @name: saveBMP
 * @desc: Nos permite guardar una imagen de tipo BMP.
 * @parameters:
 *      filename : char *
 *          - Nombre de la imagen a guardar.
 *      bInfoHeader : bmpInfoHeader *
 *          - Struct que almacena la información pertinente a la imagen, a destacar el ancho y alto en 
 * 			  píxeles.
 * 		imgdata : unsigned char *
 * 			- Arreglo de bytes que representa a la imagen.
*/
void saveBMP(char *filename, bmpInfoHeader *info, unsigned char *imgdata)
{
	bmpFileHeader header;				// Cabecera de la imagen
  	FILE *f;							// Apuntador a la imagen
  	uint16_t type;						// 2 bytes identificativos del tipo de archivo

  	f = fopen(filename, "w+");
	if(f == NULL)
	{
		perror("Error al abrir el archivo en escritura");
		exit(EXIT_FAILURE);
	}

    /* 
		Para guardar la imagen debemos crearle su información de fichero, dicha información viene específicada 
       	en cada campo el struct bmpFileHeader (excepto el tipo de archivo), al inicio declaramos una variable de tal tipo, lo que resta es 
       	ir llenando cada campo con su información correspondiente. 
	*/

    // Lo primero es el tipo de archivo, en este caso BMP distinguido por su código 0x4D42
    type = 0x4D42;
    
    // A continuación llenar el campo size, el cual es el tamaño total del archivo i.e. headers y tamaño de la imagen en sí.
  	header.size = info->imgsize + sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);

    //  Bytes reservados, no hace falta llenarlos.
  	// header.resv1=0; 
  	// header.resv2=1; 

  	// El offset será el tamaño de las dos cabeceras + 2 bytes (bytes que corresponden al tipo de archivo)
  	header.offset = 2 + sizeof(bmpFileHeader) + sizeof(bmpInfoHeader);

    // Finalmente, escribimos toda la información pertinente en el archivo y cerramos el flujo de escritura
  	fwrite(&type, sizeof(type), 1, f);
  	// Escribimos la cabecera de fichero 
  	fwrite(&header, sizeof(bmpFileHeader), 1, f);
  	// Escribimos la información básica de la imagen 
  	fwrite(info, sizeof(bmpInfoHeader), 1, f);
  	// Escribimos la imagen en sí
  	fwrite(imgdata, info->imgsize, 1, f);

  	fclose(f);
}

/**
 * @name: displayInfoBMP
 * @desc: Nos permite mostrar toda la información contenida en la estructura de cabecera de 
 *        la imagen.
 * @parameters:
 *      bInfoHeader : bmpInfoHeader *
 *          - Struct que almacena la información pertinente a la imagen, a destacar el ancho y alto en 
 * 			  píxeles.
*/
void displayInfoBMP(bmpInfoHeader *info)
{
  	printf("Tamaño de la cabecera: %u\n", info->headersize);
  	printf("Anchura: %d\n", info->width);
  	printf("Altura: %d\n", info->height);
  	printf("Planos (1): %d\n", info->planes);
  	printf("Bits por pixel: %d\n", info->bpp);
  	printf("Compresión: %d\n", info->compress);
  	printf("Tamaño de datos de imagen: %u\n", info->imgsize);
  	printf("Resolucón horizontal: %u\n", info->bpmx);
  	printf("Resolucón vertical: %u\n", info->bpmy);
  	printf("Colores en paleta: %d\n", info->colors);
  	printf("Colores importantes: %d\n", info->imxtcolors);
}
