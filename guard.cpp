/*
Author: Bella Tumaneng, Spring 2017
Base code provided by Laura Toma 

Computational Geometry Assignment 5: Art Gallery
- Compute and render the visible area of a guard (point) inside a gallery (polygon)

*/


#include "geom.h" 

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <vector>



using namespace std;



#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif





// COLOR CONSTANTS

GLfloat red[3] = {1.0, 0.0, 0.0};
GLfloat green[3] = {0.0, 1.0, 0.0};
GLfloat blue[3] = {0.0, 0.0, 1.0};
GLfloat black[3] = {0.0, 0.0, 0.0};
GLfloat white[3] = {1.0, 1.0, 1.0};
GLfloat gray[3] = {0.5, 0.5, 0.5};
GLfloat yellow[3] = {1.0, 1.0, 0.0};
GLfloat magenta[3] = {1.0, 0.0, 1.0};
GLfloat cyan[3] = {0.0, 1.0, 1.0};

GLint fillmode = 0;




// FORWARD DECLARATIONS

// helper functions
void initialize_polygon(); 
void print_polygon(vector<point2D> poly); 
void print_guard();
void check_initialization();

// graphics
void display(void);
void keypress(unsigned char key, int x, int y);
void mousepress(int button, int state, int x, int y);
void timerfunc(); 







// GLOBAL VARIABLES

const int WINDOWSIZE = 750; 
vector<point2D> gallery;
vector<point2D> vis;
point2D guard = { -10, -10} ;

// generic/testing polygon
vector<point2D>  poly;


// coordinates of last mouse click
// initialized to a point outside the window
double mouse_x = -10;
double mouse_y = -10;


// if true, clicking the mouse saves points in polygon or as guard
bool poly_init_mode = false; 
bool guard_init_mode = false;
bool initialized = false; 









/* ****************************** */
/* 			MAIN FUNCTION 		  */
/* ****************************** */
int main(int argc, char** argv) {
	
	// compute polygon of visible area
	// check if gallery not empty first
	// and if guard has been initialized

	// initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowSize(WINDOWSIZE, WINDOWSIZE);
	glutInitWindowPosition(100,100);
	glutCreateWindow(argv[0]);

	// register callback functions
	glutDisplayFunc(display); 
	glutKeyboardFunc(keypress);
	glutMouseFunc(mousepress); 
	//  glutIdleFunc(timerfunc); 

	// init GL, set background color to black
	glClearColor(0, 0, 0, 0);   
	
	// here we can enable depth testing and double buffering and so on

	
	// give control to event handler
	glutMainLoop();
	return 0;
}





/* ****************************** */
/* 		HELPER FUNCTIONS 		  */
/* ****************************** */


// INITIALIZE DEFAULT POLYGON AS MUSEUM GALLERY
void initialize_polygon() {
  
  // clear the vector just in case
  poly.clear(); 

  int n = 10; //size of polygon 
  double rad = 100; 
  double step = 2 * M_PI / n;
  point2D p; 

  for (int i = 0; i < n; i++) {
  	
  	p.x = WINDOWSIZE/2 + rad * cos (i * step); 
  	p.y = WINDOWSIZE/2 + rad * sin (i * step); 
  	
  	// insert the point in the vector of points
  	poly.push_back(p); 

  }

}





/* PRINT FUNCTIONS */

void print_polygon(vector<point2D> poly) {

	for (unsigned int i = 0; i < poly.size() - 1; i++) {
		printf("edge %d: [(%d,%d), (%d,%d)]\n", i, poly[i].x, poly[i].y, poly[i+1].x, poly[i+1].y);
  	}

	//print last edge from last point to first point 
	int last = poly.size() - 1;
    printf("edge %d: [(%d,%d), (%d,%d)]\n", last, poly[last].x, poly[last].y, poly[0].x, poly[0].y);

}

void print_guard() {
	printf("Guard: (%d, %d)\n", guard.x, guard.y);
	fflush(stdout);
}



/*	DRAW FUNCTIONS 	*/

// draws any polygon passed as parameter
void draw_polygon(vector<point2D> poly){

	if (poly.empty()) {
		return; 
	}

	//set color 
	glColor3fv(yellow);   
	
	for (int i = 0; i < poly.size() - 1; i++) {
		glBegin(GL_LINES);
		glLineWidth(5);
		glVertex2f(poly[i].x, poly[i].y); 
		glVertex2f(poly[i+1].x, poly[i+1].y);
		glEnd();
	}

	//render last segment between last point and forst point 
	int last=poly.size()-1; 
	glBegin(GL_LINES);
	glLineWidth(5);
	glVertex2f(poly[last].x, poly[last].y); 
	glVertex2f(poly[0].x, poly[0].y);
	glEnd();

}


// draws the gallery (points from global vector)
void draw_gallery() {

	if (gallery.empty()) {
		return; 
	}

	//set color 
	glColor3fv(red);   
	
	for (int i = 0; i < gallery.size() - 1; i++) {
		glBegin(GL_LINES);
		glLineWidth(5);
		glVertex2f(gallery[i].x, gallery[i].y); 
		glVertex2f(gallery[i + 1].x, gallery[i + 1].y);
		glEnd();	  
	}

	//render last segment between last point and forst point 
	int last = gallery.size() - 1; 

	glBegin(GL_LINES);
	glVertex2f(gallery[last].x, gallery[last].y); 
	glVertex2f(gallery[0].x, gallery[0].y);
	glEnd();

}

void draw_guard() {
	
	if (guard.x < 0 && guard.y < 0) {
		return;
	}
	glColor3fv(blue);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex2f(guard.x, guard.y);
	glEnd();

}




/* ****************************** */
/* 		  WINDOW FUNCTIONS 	  	  */
/* ****************************** */


/* ****************************** */
void display(void) {

	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); //clear the matrix

	// Have to map points to [-1,1] x [-1,1], default GL window
	// Assume we are the local coordinate system. 
	// Scale our window size to [0,2] x [0,2] then translate the origin
	// so that we can draw in our local coordinate system (0,0) to
	// (WINSIZE,WINSIZE), with the origin in the lower left corner.
	glScalef(2.0/WINDOWSIZE, 2.0/WINDOWSIZE, 1.0);  
	glTranslatef(-WINDOWSIZE/2, -WINDOWSIZE/2, 0); 


	// DRAW POLYGONS, GALLERY AND VIS
	// draw_polygon(poly); 
	draw_gallery();
	draw_guard();

	// execute drawing commands 
	glFlush();

}



/* ****************************** */
void keypress(unsigned char key, int x, int y) {

	switch(key) {

		case 'q':
		  exit(0);
		  break;

		// re-initialize the polygon
		case 's': 
		  gallery.clear();
		  mouse_x = mouse_y = 0; 
		  poly_init_mode = true; 
		  glutPostRedisplay();
		  break; 
		  
		case 'e': 
		  poly_init_mode = false; 
		  glutPostRedisplay();
		  break; 

		case 'g':
		  guard_init_mode = true;
		  glutPostRedisplay();
		  break;

	}

}


// INITIALIZE MUSEUM GALLERY POLYGON WITH MOUSE PRESS, STORING POINTS IN VECTOR
void mousepress(int button, int state, int x, int y) {

	if (state == GLUT_DOWN) {
		mouse_x = x;
		mouse_y = WINDOWSIZE - y; // GLUT origin is upper left, need to flip y

		if (poly_init_mode) {
			printf("gallery vertex at (x = %d, y = %d)\n", (int)mouse_x, (int)mouse_y);
		  	point2D p = { mouse_x, mouse_y }; 
		  	gallery.push_back(p);
		}

		else if (guard_init_mode) {
			printf("guard is at (x = %d, y = %d)\n", (int)mouse_x, (int)mouse_y);
			guard.x = mouse_x;
			guard.y = mouse_y;
			guard_init_mode = false;
		}

	}
	
	glutPostRedisplay();

}


/* Handler for window re-size event. Called back when the window first appears and
   whenever the window is re-sized with its new width and height */
void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
     
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   gluOrtho2D(0.0, (GLdouble) width, 0.0, (GLdouble) height); 
}





void timerfunc() {
  
  //we don't do anything here; not yet 

  glutPostRedisplay(); 

}