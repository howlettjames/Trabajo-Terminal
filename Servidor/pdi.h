/**
 * @brief: pdi.h
 * @desc: Contiene la definicion de las funciones para el 
 *        procesamiento digital de imagenes del tt1
 * 
*/
#include <stdint.h>

#ifndef PDI_H
#define PDI_H

void RGBToGray(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t width, uint32_t height);
void ajusteBrilloImagen(unsigned char * imagenGray, uint32_t width, uint32_t height, int desplazamiento);
void GrayToRGB(unsigned char *imagenRGB, unsigned char *imagenGray, uint32_t width, uint32_t height);
void smoothGauss(unsigned char *imagenGray, unsigned char *imagenGauss, uint32_t width, uint32_t height);
void *edgeDetection(void *nh);
unsigned char * reservarMemoria(uint32_t width, uint32_t height);

#endif