#include "poligonos.h"
#include "imagen_avs.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdint.h>

/*

Estructura del archivo que entra.

RES 1008 567
OJO 0.0 0.0 0.0
BACKGROUND 1.0 1.0 1.0 (RGB)
//La esfera tiene [x,y,z] radio y rgb
SPHERE 0.0 0.0 0.0 4 1.0 1.0 1.0


**/


int WIDTH = 0;
int HEIGHT = 0;

RGB background = {0.0, 0.0, 0.0};
Vector eye = {0.0, 0.0, 0.0};

// Prototipos
Scene* create_scene();
void scene_add_object(Scene* scene, Polygon* obj);
Intersection* First_Intersection(Vector* a, Vector* b, Scene* scene);
RGB* De_Que_Color(Vector a, Vector b, Scene* scene);

Scene* load_scene(const char* file_name){
	FILE* file = fopen(file_name, "r");
	if (!file) {
		printf("Error al abrir el archivo de la escena. \n");
		return NULL;
	}
	
	Scene* scene = create_scene();
	char line[256];
	while(fgets(line, sizeof(line), file)){
		if(line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;
		
		char keyword[32];
		sscanf(line, "%s", keyword);
		
		if(strcmp(keyword, "RES") == 0){
			sscanf(line, "%*s %d %d",&WIDTH,&HEIGHT);
		}
		else if(strcmp(keyword, "EYE") == 0){
			sscanf(line, "%*s %lf %lf %lf", &eye.x, &eye.y, &eye.z);
		}
		else if(strcmp(keyword, "BACKGROUND") == 0){
			sscanf(line, "%*s %lf %lf %lf", &background.R, &background.G, &background.B);
		}
		else if(strcmp(keyword, "SPHERE") == 0){
			double x, y, z, radio, r, g, b;
			sscanf(line, "%*s %lf %lf %lf %lf %lf %lf %lf", &x, &y, &z, &radio, &r, &g, &b);
			
			Polygon* obj = create_sphere(scene->Count + 1, (Vector){x, y, z}, radio, (RGB){r, g, b});
			if(obj){
				scene_add_object(scene, obj);
			}
		}
	}
	fclose(file);
	return scene;
}

Scene* create_scene(){
	Scene* scene = (Scene*)malloc(sizeof(Scene));
	if (!scene) return NULL;
	scene->head = NULL;
	scene->Count = 0;
	return scene;
}

void scene_add_object(Scene* scene, Polygon* obj){
	obj->next_p = scene->head;
	scene->head = obj;
	scene->Count++;
}

RGB* De_Que_Color(Vector a, Vector b, Scene* scene){

	Intersection* inter = First_Intersection(&a, &b, scene);

	if (inter == NULL) {
        return &background;
    }

    // Dirección del color guardado dentro del objeto.
    RGB* color = &inter->object->color;

    free(inter);

    return color;
}

Intersection* First_Intersection(Vector* a, Vector* b, Scene* scene){
	Intersection* interseccion;
	Intersection actual;
	long double tmin;
	Ray ray = {0};

	ray.origin = *a;
	ray.rayo = *b;		

	interseccion = NULL;
	tmin = INFINITY;
	Polygon* current = scene->head;
	while (current != NULL) {
		int hay = current->calculate_intersection(current->private_data, &ray, &actual);
		if (hay && actual.t < tmin) {
			tmin = actual.t;	
			actual.object = current;
			if (interseccion == NULL) {
				interseccion = malloc(sizeof *interseccion);

				if (interseccion == NULL) {
					fprintf(stderr, "No se pudo reservar la interseccion.\n");
					exit(EXIT_FAILURE);
				}
			}
			*interseccion = actual;
		}
		current = current->next_p;
	}
	return interseccion;
}

static void liberar_escena(Scene* scene) {
    if (scene == NULL) {
        return;
    }

    Polygon* actual = scene->head;

    while (actual != NULL) {
        Polygon* siguiente = actual->next_p;

        free(actual->private_data);
        free(actual);

        actual = siguiente;
    }

    free(scene);
}

int main(void) {
    Scene* scene = load_scene("scene.txt");

    if (scene == NULL) {
        fprintf(stderr, "Error cargando la escena.\n");
        return EXIT_FAILURE;
    }

    int Hres = WIDTH;
    int Vres = HEIGHT;

    if (Hres <= 0 || Vres <= 0) {
        fprintf(stderr, "La resolucion debe ser positiva.\n");
        liberar_escena(scene);
        return EXIT_FAILURE;
    }

    // Ventana de proyección en el plano z = 0
    double xmin = -300.0;
    double xmax =  300.0;

    double altura_ventana = (xmax - xmin) * ((double)Vres / Hres);

    double ymin = -altura_ventana / 2.0;
    double ymax =  altura_ventana / 2.0;

    double xe = eye.x;
    double ye = eye.y;
    double ze = eye.z;

    // Verificar el tamaño antes de reservar el framebuffer
    if ((size_t)Hres > SIZE_MAX / (size_t)Vres) {
        fprintf(stderr, "La resolucion es demasiado grande.\n");
        liberar_escena(scene);
        return EXIT_FAILURE;
    }

    size_t cantidad = (size_t)Hres * (size_t)Vres;

    if (cantidad > SIZE_MAX / sizeof(RGB)) {
        fprintf(stderr, "El framebuffer es demasiado grande.\n");
        liberar_escena(scene);
        return EXIT_FAILURE;
    }

    RGB* framebuffer = malloc(cantidad * sizeof *framebuffer);

    if (framebuffer == NULL) {
        fprintf(stderr, "No se pudo reservar el framebuffer.\n");
        liberar_escena(scene);
        return EXIT_FAILURE;
    }

    // Recorrido
    for (int i = 0; i < Hres; i++) {
        for (int j = 0; j < Vres; j++) {

            double xw = ((i + 0.5) / Hres) * (xmax - xmin) + xmin;

            double yw = ((j + 0.5) / Vres) * (ymax - ymin) + ymin;

            double zw = 0.0;

            double dx = xw - xe;
            double dy = yw - ye;
            double dz = zw - ze;

            double L = sqrt(dx * dx + dy * dy + dz * dz);

            if (!isfinite(L) || L <= 0.0) {
                fprintf(stderr, "No se pudo calcular un rayo valido.\n");
                free(framebuffer);
                liberar_escena(scene);
                return EXIT_FAILURE;
            }

            double xd = dx / L;
            double yd = dy / L;
            double zd = dz / L;

            Vector direccion = {xd, yd, zd};

            RGB* color = De_Que_Color(eye, direccion, scene);

            // j = 0 corresponde a ymin: la parte inferior.
            // AVS guarda primero la fila superior.
            size_t fila = (size_t)(Vres - 1 - j);

            size_t indice = fila * (size_t)Hres + (size_t)i;

            framebuffer[indice] = *color;
        }
    }

    // saveFile(framebuffer)
    const char* nombre_imagen = "escena.avs";

    FILE* archivo = avs_abrir(nombre_imagen, (unsigned int)Hres, (unsigned int)Vres);

    if (archivo == NULL) {
        fprintf(stderr, "No se pudo crear el archivo AVS.\n");
        free(framebuffer);
        liberar_escena(scene);
        return EXIT_FAILURE;
    }

    int escritura_correcta = 1;

    for (size_t k = 0; k < cantidad; k++) {
        RGB color = framebuffer[k];

        if (!avs_escribir_pixel(archivo, color.R, color.G, color.B)) {
            escritura_correcta = 0;
            break;
        }
    }

    int cierre_correcto = avs_cerrar(archivo);

    free(framebuffer);
    liberar_escena(scene);

    if (!escritura_correcta || !cierre_correcto) {
        fprintf(stderr, "No se pudo guardar correctamente la imagen.\n");
        return EXIT_FAILURE;
    }

    if (!avs_mostrar(nombre_imagen)) {
        fprintf(stderr, "No se pudo mostrar la imagen con ImageMagick.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
