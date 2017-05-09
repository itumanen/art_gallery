#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include "geom.h"


bool DEBUG;



// GEOMETRY FUNCTIONS

// returns the signed area of triangle abc. The area is positive if c
// is to the left of ab, and negative if c is to the right of ab
float signed_area2D(point2D a, point2D b, point2D c) {
  return( 0.5 * ((b.x-a.x) * (c.y-a.y) - (b.y-a.y) * (c.x-a.x)) );
}


// return 1 if p,q,r collinear, and 0 otherwise
bool collinear(point2D p, point2D q, point2D r) {
	return (signed_area2D(p,q,r) == 0);
}


// return 1 if c is  strictly left of ab; 0 otherwise
bool left (point2D a, point2D b, point2D c) {
  float area = signed_area2D(a,b,c);
  printf("signed area is %f\n", area);
  return (signed_area2D(a,b,c) > 0);
}

// return 1 if c is not strictly right of ab; 0 otherwise
// true if the 'turn' from a to c through b is a right turn
bool reflex_angle(point2D a, point2D b, point2D c) {
	return(!left(a,b,c));
}



// compute radial angle of point relative to guard ("origin")
// we're using atan since we can calculate tan(a) without computing
// the hypotenuse. However, the general formula for computing tan(a)
// only computes the angle that the segment [vertex, guard] forms with
// the x-axis within a single quadrant. Because of this we have to 
// add offset values in the cases where the vertex is in a quadrant
// where the x- and y-values could be negative.
float radial_angle(vertex v, point2D guard) {
	float deltX = v.x - guard.x;
	float deltY = v.y - guard.y;
	float offset = 0; 			// quadrant I, or default
	if (deltX < 0) {
		offset = 180; 			// quadrant II and III, offset = 180
	} else if (deltY < 0) {
		offset = 360; 			// quadrant IV, offset = 360
	}

	float angle = atan(deltY / deltX);
	angle = angle * 180 / PI;   // convert to degrees
	angle += offset; 			// give correct angle depending on the quadrant
	return angle;
}



// returns the index of the vertex corresponding to the endpoint of the
// edge that is being intersected by the ray
// cusp = the would-be index of the cusp in vis (needs to be added after backtrack)
vector<vertex> backtrack(vector<vertex> vis, float max_angle) {

	int index = vis.size() - 1;
	while (vis.back().angle >= max_angle && index >= 0) {
		vis.pop_back();
		index--;
	}
	return vis; 
}


// computes the intersection between two segments - from textbook
vertex compute_intersection(segment2D ray, segment2D edge) {

	vertex intersection;

	// parameters of parametric equations for ray and edge
	// numerator and denominator for readability
	float ray_param, edge_param, num, denom;

	float ax, ay, bx, by, cx, cy, dx, dy;

	ax = ray.start.x;
	ay = ray.start.y;
	bx = ray.end.x;
	by = ray.end.y;

	cx = edge.start.x;
	cy = edge.start.y;
	dx = edge.end.x;
	dy = edge.end.y;

	denom = ax * (dy - cy) +
			bx * (cy - dy) +
			dx * (by - ay) +
			cx * (ay - by);

	num = ax * (dy - cy) +
		  cx * (ay - dy) +
		  dx * (cy - ay);

	ray_param = num / denom;

	num = -1 * (ax * (cy - by) +
		  	    bx * (ay - cy) +
			    cx * (by - ay) );

	edge_param = num / denom;

	intersection.x = ax + ray_param * (bx - ax);
	intersection.y = ay + ray_param * (by - ay);

	return intersection;

}


// helper function for computing the polar angle
int compute_sigma(point2D guard, point2D prev, point2D curr) {
	printf("computing sigma,  ");
	if (left(guard, prev, curr)) {
		return 1;
	} else if (collinear(guard, prev, curr)) {
		return 0;
	} else {
		return -1;
	}
}



// polar angle (relative to the guard) is used to compare the vertices as we walk around 
// the boundary of the polygon; our main assumption is that if the polar angle at vertex 
// i is greater than the polar angle at vertex i-1, that the vertex is visible. 
float polar_angle(float prev_angle, float curr_angle, int sigma) {
	// angle formed by Prev, Guard, and Curr
	float angle_diff = fabs(curr_angle - prev_angle);
	angle_diff = min(angle_diff, (float)fabs(360 - angle_diff));
	return(prev_angle + sigma * angle_diff);
}


// COMPUTING THE VISIBLE AREA ASSUMES POINTS ARE IN COUNTER CLOCKWISE ORDER
// this was inspired by the linear algorithm by Joe and Simpson (1985)
vector<vertex> visible_area(vector<vertex> gallery, vector<vertex> vis, point2D guard) {

	// compute the radial angles of all the vertices relative to the guard,
	// keeping track of the vertices with the smallest and largest angles; 
	// sweep starts and ends at these points
	float min_angle = 360;
	int min_ang_index = 0;

	for (int i = 0; i < gallery.size(); i++) {
		gallery[i].angle = radial_angle(gallery[i], guard);
		if (gallery[i].angle < min_angle) {
			min_angle = gallery[i].angle;
			min_ang_index = i;
		} 
	}

	if (DEBUG) {
		printVertices(gallery);printf("\n");
		printf("min angle at index %d\n", min_ang_index);
		fflush(stdout);
	}
	

	// if "starting vertex" is not the first vertex that was entered,
	// move all the points entered before it to the end of the vector
	if (min_ang_index != 0) {
		vector<vertex> temp;
		for (int i = 0; i < min_ang_index; i++) {
			temp.push_back(gallery[i]);
		}
		gallery.erase(gallery.begin(), gallery.begin() + min_ang_index);
		for (int i = 0; i < temp.size(); i++) {
			gallery.push_back(temp[i]);
		}
	}

	if (DEBUG) {
		printf("printing reordered vertices\n");
		printVertices(gallery);
		fflush(stdout);
	}


	int sigma; 	// for polar angles
	bool min_ray = false; // if polygon "wraps" around the guard more than once

	gallery[0].index = 0; // just in case
	vis.push_back(gallery[0]); // for starters - this gets popped if wrong
	int forward = 0;

	for (int i = 1; i < gallery.size(); i++) {

		// just in case
		gallery[i].index = i;

		// if we hit a cusp and need to compute the intersection
		// between [guard, top of stack] and the nearest edge
		if (i < forward) {
			continue;
		} else if (i == forward) {

			// angle of current vertex > angle of last vertex in vis
			segment2D ray, edge; 
			ray.start = guard;
			ray.end.x = vis.back().x; 			ray.end.y = vis.back().y;

			edge.start.x = gallery[i - 1].x;	edge.start.y = gallery[i - 1].y;
			edge.end.x = gallery[i].x;			edge.end.y = gallery[i].y;

			// compute their intersection, and set index and angle of the new
			// vertex
			vertex intersection = compute_intersection(ray, edge);
			intersection.index = i - 1;
			intersection.angle = vis.back().angle;

			vis.push_back(intersection);

		}


		// for brevity - vertex that we're investigating
		point2D curr;
		curr.x = gallery[i].x;
		curr.y = gallery[i].y;
		float curr_angle = gallery[i].angle;


		// top of the stack - if stack is too small to compute turns, use the previous
		// vertex of the gallery for reference
		point2D prev;
		float prev_angle;

		if (vis.size() < 2) {
			prev.x = gallery[i - 1].x;
			prev.y = gallery[i - 1].y;
			prev_angle = gallery[i - 1].angle;	
		} else {
			prev.x = vis[vis.size() - 1].x;
			prev.y = vis[vis.size() - 1].y;
			prev_angle = vis[vis.size() - 1].angle;
		}
		

		/* POLAR ANGLES about the guard; for a vertex v[i]
		   v[i].angle = v[i-1].angle + angle of (v[i-1], guard, v[i])
		*/

		// compute sigma	
		sigma = compute_sigma(guard, prev, curr);

		// compute polar angle 
		curr_angle = polar_angle(prev_angle, curr_angle, sigma);

		// update value in gallery vertex, now that we have visited it
		gallery[i].angle = curr_angle;


		// if edges of the polygon have wrapped around the guard at least once, 
		// compute last intersection and break
		if (!vis.empty() && curr_angle > vis[0].angle + 360) {

			if(!min_ray) {		// if we haven't already done this
				
				segment2D ray, edge;
				ray.start = guard;
				ray.end.x = vis[0].x;		ray.end.y = vis[0].y;

				edge.start.x = curr.x; 		edge.start.y = curr.y;
				edge.end.x = prev.x; 		edge.end.y = prev.y;

				// compute intersection
				vertex intersection = compute_intersection(ray, edge);
				intersection.index = i - 1;
				intersection.angle = min_angle + 360;

				vis.push_back(intersection);

				min_ray = true;
			}

			continue;
		}


		// ASSUMPTION: next angle is larger than angle at top of stack
		// curr_angle >= vis.back().angle
		if (curr_angle >= prev_angle) {
			printf("adding vertex at index %d\n", i);
			vis.push_back(gallery[i]);
			continue;
		} 


		// find the vertex that comes before prev - if there is another vertex
		// before prev on the stack, use that one; otherwise take the vertex that
		// comes before it in the gallery vector. 
		// we'll use this to determine if prev is a left or right turn.
		point2D v;
		int v_index;
		if (vis.size() < 2) {
			v_index = i - 2;
			if (i == 1) {
				v_index = gallery.size() - 1;
			}
			v.x = gallery[v_index].x;
			v.y = gallery[v_index].y;		
		} else {
			v_index = vis.size() - 2;
			v.x = vis[v_index].x;
			v.y = vis[v_index].y;
		}


		// SPECIAL CASE CUSP
		// current angle is smaller than the angle at the bottom of the stack; since
		// the polygon is not complex and we are moving in ccw order, then if curr is 
		// to the left of the first vertex and the guard (generally this means that 
		// curr_angle < 0 and the polygon has yet to 'wrap around' the guard, curr is
		// closer to the guard than the bottom of the stack and therefore visible. Thus
		// we have to clear the stack and set the bottom of it to curr. 
		if(curr_angle < min_angle && left(prev, guard, curr)) {
			printf("special case cusp\n");
			vis = backtrack(vis, curr_angle);
			min_angle = curr_angle;
			vis.push_back(gallery[i]);
			continue;
		}
		

		// if left turn at prev
		// compare to top of stack
		if (left(v, prev, curr)) { 	// curr is a cusp

			// backtrack and pop off vertices that are no longer visible
			vis = backtrack(vis, curr_angle);

			// if the stack is not empty, shoot a ray through the current vertex and compute
			// its intersection with the edge that starts with the vertex at the top of the stack
			if (!vis.empty()) {

				segment2D ray, edge;
				ray.start = guard;
				ray.end.x = gallery[i].x;		ray.end.y = gallery[i].y;

				// index of the vertex at the top of the stack, within the gallery vector
				int gal_index = vis.back().index + 1;
				
				edge.start.x = vis.back().x; 		edge.start.y = vis.back().y;
				edge.end.x = gallery[gal_index].x; 	edge.end.y = gallery[gal_index].y;

				// compute intersection, then push it on the stack
				vertex intersection = compute_intersection(ray, edge);
				intersection.index = i;
				intersection.angle = curr_angle;

				vis.push_back(intersection);
			}

			// add current vertex
			vis.push_back(gallery[i]);

		} else if (curr_angle > 0){ 	// curr is a not visible

			// continue around the boundary of the gallery until a new visible vertex is found 
			// (gallery[forward+1]). We will then shoot a ray from the guard through the current 
			// vertex and compute its intersection with the edge formed by gallery[forward] and the 
			// visible vertex. 
			// Since we're using a for loop, we have a check at the beginning of the next iterations
			// that computes this.
			forward = i; 
			while(gallery[forward].angle < vis.back().angle && forward < gallery.size()) {
				forward++;
			}

		}

	}

	printf("\nprinting vis vertices\n");
	printVertices(vis);
	fflush(stdout);
	// return the final vector
	return vis;
}




// HELPER FUNCTIONS

void printPoint(point2D p) {
	printf("(%f, %f)\n", p.x, p.y);
	fflush(stdout);
}

void printVertices(vector<vertex> v) {
	for (int i = 0; i < v.size(); i++) {
		printf("%d: (%f, %f) angle: %f\n", i, v[i].x, v[i].y, v[i].angle);
	}
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
