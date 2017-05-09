/* 
Base code: Laura Toma
*/

#include <vector>

using namespace std;

#ifndef __geom_h
#define __geom_h

#define EPSILON 0.0001
#define PI 3.14159265


typedef struct _point2d {
  float x,y; 
} point2D;


typedef struct _segment2D {
    point2D start, end;
} segment2D;



typedef struct _vertex {
	float x, y, angle;
	int index;
} vertex;



typedef struct _angle_sort {
	bool operator() (const vertex &v1, const vertex &v2) { return(v1.angle < v2.angle); }
} angle_sort;




// geometry functions
float signed_area2D(point2D a, point2D b, point2D c);
bool collinear(point2D p, point2D q, point2D r);
bool left (point2D a, point2D b, point2D c);
bool reflex_angle(point2D a, point2D b, point2D c);
float radial_angle(vertex v, point2D guard);
vector<vertex> backtrack(vector<vertex> vis, float max_angle);
vertex compute_intersection(segment2D ray, segment2D edge);
int compute_sigma(point2D guard, point2D prev, point2D curr);
float polar_angle(float prev_angle, float curr_angle, int sigma);
vector<vertex> visible_area(vector<vertex> gallery, vector<vertex> vis, point2D guard);


// helper functions to operate on points and segments
void printPoint(point2D p);
void printVertices(vector<vertex> v);
void printSegment(segment2D s);
bool equals(point2D a, point2D b);

#endif