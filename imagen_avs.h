#ifndef IMAGEN_AVS_H
#define IMAGEN_AVS_H

#include <stdio.h>

// Devuelve NULL si no puede crear el archivo
FILE* avs_abrir(const char* nombre, unsigned int ancho, unsigned int alto);

// Los colores
int avs_escribir_pixel(FILE* archivo, double rojo, double verde, double azul);

// Cerrar el .avs
int avs_cerrar(FILE* archivo);

// Abre ImageMagick y espera hasta que se cierre el visor
int avs_mostrar(const char* nombre);

#endif