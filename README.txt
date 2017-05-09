Author: Bella Tumaneng, Spring 2017
Instructor: Laura Toma

Computational Geometry Assignment 5: Art Gallery
- Compute and render the visible area of a guard (point) inside a gallery (polygon).
- Implements the linear algorithm by Joe and Simpson (1985), inspired by this description: http://cs.smith.edu/~jorourke/books/ArtGalleryTheorems/Art_Gallery_Chapter_8.pdf
- Assumes that the points are entered in counter clockwise order, and that the guard is entered inside the polygon. 


USAGE: ./guard 

KEY PRESSES:
q: quit
s: start, reinitialize the gallery. Expected: clicking points in the window designates vertices for the gallery; the points are saved in the global vector. 
e: end, signal that the last vertex has been clicked. This will draw the edge between the first and last vertices of the gallery.
g: guard, can only be used once 'e' has been entered. User can click on a new position of the guard after entering 'g' every time. 
v: calculate visible area of the guard "manually."

DATA STRUCTURES:
- vertex: stores (x,y) coordinates of a point, the radial angle of the point relative to the guard, and the "index," which corresponds to the order in which the points were entered.

TODO:
- process the last edge, formed by the last vertex and the vertex at 0. 
- check if the guard is inside the polygon

