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



// compute radial angle of point relative to guard
// atan computes angle in radians
// if the vertex of the gallery is lower than the guard in the window
// (x-value is smaller), then the radial angle is in [-180, 0]; to 
// move all angles into the range [0, 360] we have to add 360 to all
// negative angle values
float radial_angle(vertex v, point2D guard) {
	float angle = atan( ( float(v.y - guard.y) / float(v.x - guard.x)) );
	angle = angle * 180 / PI; // convert to degrees
	if (angle < 0) { 
		angle += 360;
	}
	return angle;
}


// check ordering of points - if entered in CCW order, make sure everything
// before startIndex is moved to the end of the vector
// if the user entered the points in CW order, reverse the vector and then
// move everything to one side of startIndex.
// vector<vertex> reorder(vector<vertex> gallery, startIndex) {
// 	int size = gallery.size();
// 	gallery.resize(size); // avoid unnecessary expansion when moving points around
// 	return gallery;
// }



bool ccw(int min_ang_index, int max_ang_index) {
	if (!min_ang_index) { 	//starting index is 0
		return true;
	} else {
		return (min_ang_index > max_ang_index);
	}
}


// assumes that the vertex angle was set before visible was called
bool visible(vertex v, float radMax) {
	return (v.angle > radMax);
}

// returns the index of the vertex corresponding to the endpoint of the
// edge that is being intersected by the ray
// cusp = the would-be index of the cusp in vis (needs to be added after backtrack)
int backtrack(vector<vertex> vis, int cusp, segment2D ray) {

	float max_angle = vis[cusp].angle;
	int index = cusp - 1;
	while (vis[index].angle > max_angle && index > 0) {
		index--;
	}
	return index; // TODO also update max_angle in visible method
}




vector<vertex> visible_area(vector<vertex> gallery, vector<vertex> vis, point2D guard) {

	// compute the radial angles of all the vertices relative to the guard,
	// keeping track of the vertices with the smallest and largest angles; 
	// sweep starts and ends at these points
	float min_angle = 360;
	float max_angle = 0;
	int min_ang_index = 0;
	int max_ang_index = 0;
	for (int i = 0; i < gallery.size(); i++) {
		gallery[i].angle = radial_angle(gallery[i], guard);
		if (gallery[i].angle < min_angle) {
			min_angle = gallery[i].angle;
			min_ang_index = i;
		} else if (gallery[i].angle > max_angle) {
			max_angle = gallery[i].angle;
			max_ang_index = i;
		}
	}

	// vertex with smallest radial angle (at an index i) is the starting point for the 
	// radial/directional sweep. the sweep is performed in counter clockwise order - thus, 
	// if the points were entered by the user in ccw order, then this last vertex should 
	// be at index i-1. If this is not the case then we know that the points were entered 
	// in clockwise order, and have to reverse the order of the points in order to compare
	// the order in which they were entered to the radial order. 
	if (ccw(min_ang_index, max_ang_index)) {
		printf("COUNTER CLOCKWISE\n");
	} else {
		printf("CLOCKWISE\n");
		// TODO rearrange the points
	}

	float radMax = 0;
	int vertexMax; // index of vertex with max angle
	bool trace_ray; // if intersection needs to be computed

	for (int i = 0; i < gallery.size(); i++) {

		if (gallery[i].angle < radMax) {
			trace_ray = true;
			continue;
		}

		// angle <= radMax
		// compute intersection between ray first encountered edge of polygon
		// add to vector of vertices before the vertex that updated the angle
		if (trace_ray) {
			
			// segment2D ray, edge; 
			// ray.start = guard;
			// ray.end = vertexMax;
			// edge.start = gallery[i];
			// edge.end = gallery[i-1];
			
			// point2D intersection;
			// // intersection of linear equations - results in floats then cast to ints

			// vis.push_back(intersection);
			trace_ray = false;
		}

		// update vertex of max angle
		vertexMax = i; 
		vis.push_back(gallery[vertexMax]);

	}

	return vis;
}






// HELPER FUNCTIONS

void printPoint(point2D p) {
	printf("(%f, %f)\n", p.x, p.y);
	fflush(stdout);
}

void printSegment(segment2D s) {
	printf("start (%f, %f)  end (%f, %f)\n", s.start.x, s.start.y, s.end.x, s.end.y);
	fflush(stdout);
}

// returns true if a and b have the same coordinates
bool equals(point2D a, point2D b) {
	return ((a.x == b.x) && (a.y == b.y));
}
