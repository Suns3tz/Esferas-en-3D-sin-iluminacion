#ifndef POLYGON_H
#define POLYGON_H

typedef struct {
	int id;
	Polygon* next_p;
	void* private_data;
	int (*calculate_intersection)
	(void* private_data, 
	const Ray* ray, 
	Intersection* intersection);
} Polygon;

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
} Ray;

typedef struct{
	double radio;
	Vector center;
} Sphere;



#endif
