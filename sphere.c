#include "poligonos.h"
#include <stdlib.h>
#include <stdio.h>


Polygon* create_sphere(int id, Vector center, double radio, RGB color){
	Polygon* obj = (Polygon)malloc(sizeof(Polygon));
	if (!obj) return NULL;
	
	Sphere* sphere = (Sphere*)malloc(sizeof(Sphere));
	if(!sphere) {
		free(obj);
		return NULL;
	}
	
	sphere->center = center;
	sphere->radio = radio;
	
	obj->id = id;
	obj->color = color;
	obj->next_p = NULL;
	obj->private_data = sphere;
	obj->calculate_intersection = sphere_intersect;
	
	return obj;
}

//Falta sphere_intersect;
