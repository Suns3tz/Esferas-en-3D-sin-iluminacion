#ifndef POLYGON_H
#define POLYGON_H

// Declaración adelantada porque Polygon se referencia a sí mismo
typedef struct Polygon Polygon;

typedef struct {
	double R;
	double G;
	double B;
} RGB;

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

typedef struct {
	double t;
	Polygon* object;
	Vector point;
	Vector object_point;
} Intersection;

struct Polygon {
    int id;
    RGB color;
    Polygon* next_p;
    double light_kd; 
    double light_ka;
    void* private_data;
    int (*calculate_intersection)(
        void* private_data,
        const Ray* ray,
        Intersection* intersection
    );
    Vector (*calculate_normal)(
    Vector* point,
    void* private_data);
};

typedef struct {
	double light_Ip;
	Vector PoL;
	double c1;
	double c2;
	double c3;
} Light;

typedef struct {
	double light_amb;
	Polygon* head;
	Light** lights;
	int N_lights;
	int Count;
} Scene;

typedef struct{
	double radio;
	Vector center;
} Sphere;

int sphere_intersect
	(void* private_data, 
	const Ray* ray, 
	Intersection* intersection);
	
Vector normal_sphere(Vector* point, void* private_data);


Polygon* create_sphere(int id, Vector center, double radio, RGB color, double Ka, double Kd);

#endif
