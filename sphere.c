#include "poligonos.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>


Polygon* create_sphere(int id, Vector center, double radio, RGB color){
	Polygon* obj = malloc(sizeof *obj);
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

int sphere_intersect(void* private_data, const Ray* ray, Intersection* intersection){
	Sphere* sphere = (Sphere*)private_data;

	// V = VE - VC 
	double Vx = ray->origin.x - sphere->center.x;
	double Vy = ray->origin.y - sphere->center.y;
	double Vz = ray->origin.z - sphere->center.z;

	// a = 1
	double a = ray->rayo.x * ray->rayo.x + ray->rayo.y * ray->rayo.y + ray->rayo.z * ray->rayo.z;

	double b = 2.0 * (ray->rayo.x * Vx + ray->rayo.y * Vy + ray->rayo.z * Vz);

	double c = Vx * Vx + Vy * Vy + Vz * Vz - sphere->radio * sphere->radio;

	double delta = b * b - 4 * a * c;

	if (delta < 0) {
		return 0; // no hay interseccion
	}

	double sqrt_delta = sqrt(delta);
	double t1 = (-b - sqrt_delta) / (2 * a);
	double t2 = (-b + sqrt_delta) / (2 * a); 

	double t;
	if (t1 > 0) {
		t = t1;              // 0 < t1 < t2: esfera al frente del ojo
	} else if (t2 > 0) {
		t = t2;               // t1 < 0 < t2: ojo dentro de la esfera
	} else {
		return 0;             // t1 < t2 < 0: esfera atras del ojo
	}

	intersection->t = t;
	intersection->point.x = ray->origin.x + t * ray->rayo.x;
	intersection->point.y = ray->origin.y + t * ray->rayo.y;
	intersection->point.z = ray->origin.z + t * ray->rayo.z;

	return 1;
}
