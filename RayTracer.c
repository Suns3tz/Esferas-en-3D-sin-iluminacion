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
SPHERES 8
//La esfera tiene [x,y,z] radio y rgb
SPHERE 0.0 0.0 0.0 4 1.0 1.0 1.0


**/

//Falta parsear el archivo

RGB* background;
background->R = 1.0f;
background->G = 1.0f;
background->B = 1.0f;

RGB* De_Que_Color(Vector a, Vector b, Scene* scene);
Intersection* First_Intersection(Vector* a, Vector* b, Scene* scene);

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
    Scene* scene;
    scene = create_scene();
}
