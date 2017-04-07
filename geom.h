/* 
Base code: Laura Toma
*/

#include <vector>

using namespace std;

#ifndef __geom_h
#define __geom_h


typedef struct _point2d {
  int x,y; 
} point2D;


typedef struct _segment2D {
    point2D start, end;
} segment2D;


typedef struct _tri2D  {
    point2D a, b, c;
} tri2D;


typedef struct _polygon {
	int numPoints;
	vector<point2D> v; // vertices
	vector<segment2D> e; // edges
} polygon;




// geometry functions
int signed_area2D(point2D a, point2D b, point2D c);
bool collinear(point2D p, point2D q, point2D r);
bool left (point2D a, point2D b, point2D c);
bool reflex_angle(point2D a, point2D b, point2D c);
vector<point2D> visible_area(vector<point2D> gallery, vector<point2D> vis, point2D guard);


// helper functions to operate on points and segments
void printPoint(point2D p);
void printSegment(segment2D s);
bool equals(point2D a, point2D b);

#endif