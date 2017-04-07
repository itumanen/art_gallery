#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "geom.h"




// GEOMETRY FUNCTIONS

// returns the signed area of triangle abc. The area is positive if c
// is to the left of ab, and negative if c is to the right of ab
int signed_area2D(point2D a, point2D b, point2D c) {
  return( 0.5 * ((b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x)) );
}


// return 1 if p,q,r collinear, and 0 otherwise
bool collinear(point2D p, point2D q, point2D r) {
	return (signed_area2D(p,q,r) == 0);
}


// return 1 if c is  strictly left of ab; 0 otherwise
bool left (point2D a, point2D b, point2D c) {
  return (signed_area2D(a,b,c) > 0);
}


bool reflex_angle(point2D a, point2D b, point2D c) {
	return(!left(a,b,c));
}

vector<point2D> visible_area(vector<point2D> gallery, vector<point2D> vis, point2D guard) {

	int size = vector.size();
	vector.resize(size); // avoid unnecessary expansion when moving points around
	// TODO rearrange the points

	float radMax = 0;
	point2D vertexMax; // point at max angle
	bool trace_ray; // if intersection needs to be computed

	for (int i = 0; i < gallery.size(); i++) {
		// calculate angle
		float angle;

		if (angle < radMax) {
			trace_ray = true;
			continue;
		}

		// angle <= radMax
		// compute intersection between ray first encountered edge of polygon
		// add to vector of vertices before the vertex that updated the angle
		if (trace_ray) {
			
			segment2D ray, edge; 
			ray.start = guard;
			ray.end = vertexMax;
			edge.start = gallery[i];
			edge.end = gallery[i-1];
			
			point2D intersection;
			// intersection of linear equations - results in floats then cast to ints

			vis.push_back(intersection);
			trace_ray = false;
		}

		// update vertex of max angle
		vertexMax = gallery[i]; 
		vis.push_back(vertexMax);

	}

	return vis;
}


// HELPER FUNCTIONS

void printPoint(point2D p) {
	printf("(%d, %d)\n", p.x, p.y);
	fflush(stdout);
}

void printSegment(segment2D s) {
	printf("start (%d, %d)  end (%d, %d)\n", s.start.x, s.start.y, s.end.x, s.end.y);
	fflush(stdout);
}

// returns true if a and b have the same coordinates
bool equals(point2D a, point2D b) {
	return ((a.x == b.x) && (a.y == b.y));
}
