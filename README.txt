Author: Bella Tumaneng, Spring 2017
Instructor: Laura Toma

Computational Geometry Assignment 5: Art Gallery
- Compute and render the visible area of a guard (point) inside a gallery (polygon)

USAGE: ./guard 

KEY PRESSES:
q: quit
s: start, reinitialize the gallery. Expected: clicking points in the window designates vertices
	for the gallery; the points are saved in the global vector. 
e: end, signal that the last vertex has been clicked.
g: guard, can only be used after 'e', can be used to update position of the guard. 

DATA STRUCTURES:

ALGORITHMS:
- Computing the visible polygon
- Triangulating the visible polygon

TASKS/TODO PART 1:
- Construct and render a polygon based on where the user clicks in the window
	* Have user enter the number of vertices --> maximum number of clicks to form polygon
- Draw a point to represent the guard, based on user input (clicking)
	* Method to test if the guard is inside the polygon
		- Error messages
		- Still prompt for point
- Compute the visible area of this point within the polygon; the result should be another polygon
- Triangulate the visible area polygon, in case area is not convex polygon
- Method to test whether a polygon is "simple" (edges only intersect in vertices)

NOTES ON GRAPHICS:
- Our coordinate system is (0,0) x (WINDOWSIZE,WINDOWSIZE) where the origin is the lower left corner
	* GLUT sets origin in the upper left, so have to reflect y-coordinate in mousepress
	* The default GL window is [-1,1]x[-1,1] with the origin in the center, so we have to map the points
	  to [-1,1] x [-1,1] 

- Museum boundaries: glPolygonMode(GL_LINE);
- Visible area: glPolygonMode(GL_FILL);
	* Note: OpenGL can only fill convex polygons