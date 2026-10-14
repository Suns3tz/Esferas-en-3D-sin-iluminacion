#include "imagen_avs.h"

#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Escribe un entero de 32 bits en orden big-endian
static int escribir_entero_32(FILE* archivo, uint32_t valor) {
    unsigned char bytes[4] = {
        (unsigned char)((valor >> 24) & 0xFF),
        (unsigned char)((valor >> 16) & 0xFF),
        (unsigned char)((valor >> 8) & 0xFF),
        (unsigned char)(valor & 0xFF)
    };

    return fwrite(bytes, 1, sizeof(bytes), archivo) == sizeof(bytes);
}

// Convierte un componente entre 0.0 y 1.0 a un byte
static unsigned char convertir_color(double valor) {
    if (!(valor > 0.0)) {
        return 0;
    }

    if (valor >= 1.0) {
        return 255;
    }

    return (unsigned char)(valor * 255.0 + 0.5);
}

FILE* avs_abrir(const char* nombre, unsigned int ancho, unsigned int alto) {
    if (nombre == NULL || nombre[0] == '\0' ||
        ancho == 0 || alto == 0 ||
        (uintmax_t)ancho > UINT32_MAX ||
        (uintmax_t)alto > UINT32_MAX) {
        return NULL;
    }

    FILE* archivo = fopen(nombre, "wb");

    if (archivo == NULL) {
        return NULL;
    }

    // Cabecera del AVS: 4 bytes de ancho y 4 bytes de alto
    if (!escribir_entero_32(archivo, (uint32_t)ancho) ||
        !escribir_entero_32(archivo, (uint32_t)alto)) {
        fclose(archivo);
        return NULL;
    }

    return archivo;
}

int avs_escribir_pixel(FILE* archivo, double rojo, double verde, double azul) {
    if (archivo == NULL) {
        return 0;
    }

    unsigned char pixel[4] = {
        255,
        convertir_color(rojo),
        convertir_color(verde),
        convertir_color(azul)
    };

    return fwrite(pixel, 1, sizeof(pixel), archivo) == sizeof(pixel);
}

int avs_cerrar(FILE* archivo) {
    if (archivo == NULL) {
        return 0;
    }

    return fclose(archivo) == 0;
}

int avs_mostrar(const char* nombre) {
    if (nombre == NULL || nombre[0] == '\0') {
        return 0;
    }

    pid_t proceso = fork();

    if (proceso < 0) {
        return 0;
    }

    if (proceso == 0) {
        // ImageMagick 6 o instalaciones con "display"
        execlp("display", "display", nombre, (char*)NULL);

        // Si no se encontró el ejecutable, probar ImageMagick 7
        if (errno == ENOENT) {
            execlp("magick", "magick", "display",
                   nombre, (char*)NULL);
        }

        _exit(127);
    }

    int estado;
    pid_t resultado;

    do {
        resultado = waitpid(proceso, &estado, 0);
    } while (resultado < 0 && errno == EINTR);

    if (resultado < 0) {
        return 0;
    }

    return WIFEXITED(estado) && WEXITSTATUS(estado) == 0;
}