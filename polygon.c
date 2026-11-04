#include "poligonos.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


static void free_polygon_data(void* private_data){
	PolygonData* polygon = (PolygonData*)private_data;
	if (!polygon) return;
	free(polygon->vertices);
	free(polygon->u);
	free(polygon->v);
	free(polygon);
}

Polygon* create_polygon(int id, Vector* vertices, int n, RGB color, double Kd, double Ka, double Kn, double Ks){
	if (!vertices || n < 3) {
		fprintf(stderr, "Poligono %d: necesita al menos 3 vertices.\n", id);
		return NULL;
	}

	Polygon* obj = malloc(sizeof *obj);
	PolygonData* polygon = calloc(1, sizeof *polygon);
	if (!obj || !polygon) {
		free(obj);
		free(polygon);
		return NULL;
	}

	polygon->n = n;
	polygon->vertices = malloc(n * sizeof *polygon->vertices);
	polygon->u = malloc(n * sizeof *polygon->u);
	polygon->v = malloc(n * sizeof *polygon->v);
	if (!polygon->vertices || !polygon->u || !polygon->v) {
		free_polygon_data(polygon);
		free(obj);
		return NULL;
	}
	memcpy(polygon->vertices, vertices, n * sizeof *vertices);

	// Plano: N = (P1 - P0) x (P2 - P0) = [A, B, C]
	Vector P0 = vertices[0];
	Vector P1 = vertices[1];
	Vector P2 = vertices[2];

	double ux = P1.x - P0.x;
	double uy = P1.y - P0.y;
	double uz = P1.z - P0.z;
	double vx = P2.x - P0.x;
	double vy = P2.y - P0.y;
	double vz = P2.z - P0.z;

	double A = uy * vz - uz * vy;
	double B = uz * vx - ux * vz;
	double C = ux * vy - uy * vx;
	double L = sqrt(A * A + B * B + C * C);

	// P0, P1 y P2 colineales
	if (L < EPSILON) {
		fprintf(stderr, "Poligono %d: los primeros 3 vertices son colineales.\n", id);
		free_polygon_data(polygon);
		free(obj);
		return NULL;
	}

	// Normalizar
	polygon->A = A / L;
	polygon->B = B / L;
	polygon->C = C / L;
    
	polygon->D = -(polygon->A * P0.x + polygon->B * P0.y + polygon->C * P0.z);

	
	double absA = fabs(polygon->A);
	double absB = fabs(polygon->B);
	double absC = fabs(polygon->C);
	if (absA >= absB && absA >= absC) {
		polygon->ignored_axis = 0;           // sin x 
	} else if (absB >= absC) {
		polygon->ignored_axis = 1;           // sin y 
	} else {
		polygon->ignored_axis = 2;           // sin z 
	}

	// Aplastar los vertices a (u, v)
	for (int k = 0; k < n; k++) {
		Vector p = vertices[k];
		switch (polygon->ignored_axis) {
			case 0:  polygon->u[k] = p.y; polygon->v[k] = p.z; break;
			case 1:  polygon->u[k] = p.x; polygon->v[k] = p.z; break;
			default: polygon->u[k] = p.x; polygon->v[k] = p.y; break;
		}
	}

	obj->id = id;
	obj->color = color;
	obj->next_p = NULL;
	obj->private_data = polygon;
	obj->calculate_intersection = polygon_intersect;
	obj->calculate_normal = normal_polygon;
	obj->free_data = free_polygon_data;
	obj->light_ka = Ka;
	obj->light_kd = Kd;
	obj->light_kn = Kn;
	obj->light_ks = Ks;
	return obj;
}

int polygon_intersect(void* private_data, const Ray* ray, Intersection* intersection){
	PolygonData* polygon = (PolygonData*)private_data;

	// interseccion rayo/plano
	
	double denom = polygon->A * ray->rayo.x + polygon->B * ray->rayo.y + polygon->C * ray->rayo.z;

	if (fabs(denom) < EPSILON) {
		return 0;             
	}

	double t = -(polygon->A * ray->origin.x + polygon->B * ray->origin.y + polygon->C * ray->origin.z + polygon->D) / denom;

	if (t <= EPSILON) {
		return 0;            
	}

	Vector P;
	P.x = ray->origin.x + t * ray->rayo.x;
	P.y = ray->origin.y + t * ray->rayo.y;
	P.z = ray->origin.z + t * ray->rayo.z;

	
	// Aplastar el punto igual que los vertices
	double ui, vi;
	switch (polygon->ignored_axis) {
		case 0:  ui = P.y; vi = P.z; break;
		case 1:  ui = P.x; vi = P.z; break;
		default: ui = P.x; vi = P.y; break;
	}

	// Contar paredes
	int contador_paredes = 0;
	for (int k = 0; k < polygon->n; k++) {
		int sig = (k + 1) % polygon->n;

		double ua = polygon->u[k]   - ui;
		double va = polygon->v[k]   - vi;
		double ub = polygon->u[sig] - ui;
		double vb = polygon->v[sig] - vi;

		
		int v_cambia_signo = (va >= 0 && vb < 0) || (va < 0 && vb >= 0);

		// Rechazo trivial
		if ((va >= 0 && vb >= 0) || (va < 0 && vb < 0)) {
			continue;
		}

		// Rechazo trivial
		if (ua < 0 && ub < 0) {
			continue;
		}

		// Aceptacion trivial
		if (ua >= 0 && ub >= 0 && v_cambia_signo) {
			contador_paredes++;
			continue;
		}

		// Caso complicado
		if (((ua >= 0 && ub < 0) || (ua < 0 && ub >= 0)) && v_cambia_signo) {
			// Donde corta la pared al eje u
			double uc = ua - va * (ub - ua) / (vb - va);
			if (uc > 0) {
				contador_paredes++;
			}
		}
	}

	// Impar = adentro, par = afuera
	if (contador_paredes % 2 == 0) {
		return 0;
	}

	intersection->t = t;
	intersection->point = P;

	return 1;
}

Vector normal_polygon(Vector* point, void* private_data){
	(void)point;              
	PolygonData* polygon = (PolygonData*)private_data;
	Vector normal = {polygon->A, polygon->B, polygon->C};
	return normal;
}
