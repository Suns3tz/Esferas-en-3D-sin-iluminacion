#include "poligonos.h"
#include "imagen_avs.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

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

RGB* background = {0.0f, 0.0f, 0.0f};
Vector* eye = {0.0f, 0.0f, 0.0f};

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
	scene->object_count = 0;
	return scene;
}

void scene_add_object(Scene* scene, Polygon* obj){
	obj->next_p = scene->head;
	scene->head = obj;
	scene->Count++;
}

RGB* De_Que_Color(Vector a, Vector b, Scene* scene){
	RGB* color;
	Intersection* inter;
	
	inter = First_Intersection(&a, &b, scene);
	if(!inter){
		color = background;
	}else{
		color = inter->object.color;
		free(inter);
	}
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
			if (!interseccion) interseccion = (Intersection*)malloc(sizeof(Intersection));
			*interseccion = actual;
		}
		current = current->next_p;
	}
	return interseccion;
}


int main(){
    Scene* scene = load_scene("scene.txt");
    if(!scene) {
		printf("Error cargando la escena \n");
		return 1;
	}
	
	return 0
}
