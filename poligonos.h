#ifndef POLYGON_H
#define POLYGON_H

#include "sphere.c"

typedef struct {
	Polygon* head;
	int Count;
} Scene;

typedef struct {
	int id;	
	RGB* color;
	Polygon* next_p;
	void* private_data;
	int (*calculate_intersection)
	(void* private_data, 
	const Ray* ray, 
	Intersection* intersection);
} Polygon;

typedef struct {
	double R;
	double G;
	double B;
}RGB;


typedef struct {
	double t;
	Polygon* object;
	Vector point;
} Intersection;

typedef struct {
	double x;
	double y; 
	double z;
} Vector;

typedef struct {
	Vector origin;
	Vector destiny;
	Vector rayo;
} Ray;

typedef struct{
	double radio;
	Vector center;
} Sphere;

int sphere_intersect
	(void* private_data, 
	const Ray* ray, 
	Intersection* intersection);

Polygon* create_sphere(int id, Vector center, double radio);

#endif
