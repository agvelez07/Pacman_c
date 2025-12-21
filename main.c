
#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdio.h>
#include "pm-maps.h"

/*GLfloat vertices[][3] = {{-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };
*/

GLfloat vertices[][3] = { {-1.0,-1.0,-0.1},{1.0,-1.0,-0.1},
{1.0,1.0,-0.1}, {-1.0,1.0,-0.1}, {-1.0,-1.0,0.1},
{1.0,-1.0,0.1}, {1.0,1.0,0.1}, {-1.0,1.0,0.1} };

GLfloat normals[][3] = { {-1.0,-1.0,-1.0},{1.0,-1.0,-1.0},
{1.0,1.0,-1.0}, {-1.0,1.0,-1.0}, {-1.0,-1.0,1.0},
{1.0,-1.0,1.0}, {1.0,1.0,1.0}, {-1.0,1.0,1.0} };

GLfloat colors[][3] = { {0.0,0.0,0.0},{1.0,0.0,0.0},
{1.0,1.0,0.0}, {0.0,1.0,0.0}, {0.0,0.0,1.0},
{1.0,0.0,1.0}, {1.0,1.0,1.0}, {0.0,1.0,1.0} };


// PACMAN - MAP (change the viewer position)
//static GLdouble viewer[] = { 1.0, -2.0, 3.0 }; /* initial viewer location */
static GLdouble viewer[] = { 0.0, 0.5, 2.5 }; /* initial viewer location */
char** map = NULL;

void polygon(int a, int b, int c, int d)
{
	/* draw a polygon via list of vertices */

	glBegin(GL_POLYGON);
		//glColor3fv(colors[a]);
		glNormal3fv(normals[a]);
		glVertex3fv(vertices[a]);
		//glColor3fv(colors[b]);
		glNormal3fv(normals[b]);
		glVertex3fv(vertices[b]);
		//glColor3fv(colors[c]);
		glNormal3fv(normals[c]);
		glVertex3fv(vertices[c]);
		//glColor3fv(colors[d]);
		glNormal3fv(normals[d]);
		glVertex3fv(vertices[d]);
	glEnd();
}


void colorcube(void)
{
	/* map vertices to faces */

	glColor3fv(colors[1]);
	polygon(0, 3, 2, 1);
	glColor3fv(colors[2]);
	polygon(2, 3, 7, 6);
	glColor3fv(colors[3]);
	polygon(0, 4, 7, 3);
	glColor3fv(colors[4]);
	polygon(1, 2, 6, 5);
	glColor3fv(colors[5]);
	polygon(4, 5, 6, 7);
	glColor3fv(colors[7]);
	polygon(0, 1, 5, 4);
}


void eixos() {
	glBegin(GL_LINES);
		glColor3f(1.0, 1.0, 0.0);
		glVertex3f(-2, 0, 0);
		glVertex3f(2, 0, 0);

		glVertex3f(2, 0, 0);
		glVertex3f(1.7, 0.3, 0);
		glVertex3f(2, 0, 0);
		glVertex3f(1.7, -0.3, 0);


		glColor3f(0.0, 1.0, 0.0);
		glVertex3f(0, -2, 0);
		glVertex3f(0, 2, 0);

		glVertex3f(0, 2, 0);
		glVertex3f(0.3, 1.7, 0);
		glVertex3f(0, 2, 0);
		glVertex3f(-0.3, 1.7, 0);


		glColor3f(0.0, 0.0, 1.0);
		glVertex3f(0, 0, -2);
		glVertex3f(0, 0, 2);

		glVertex3f(0, 0, 2);
		glVertex3f(0.3, 0, 1.7);
		glVertex3f(0, 0, 2);
		glVertex3f(-0.3, 0, 1.7);

	glEnd();
}


// PACMAN - MAP
void drawRawMap(char** m) {
	int i, j;
	int maxSize = xTabSize() > yTabSize() ? xTabSize() : yTabSize();

	glPushMatrix();

	glScalef(1.0 / maxSize, 1.0 / maxSize, 1.0 / maxSize);
	glTranslatef(-(xTabSize() - 1), -(yTabSize() - 1), 0.0);

	for (i = 0; i < xTabSize(); i++)
		for (j = 0; j < yTabSize(); j++) {
			if (m[i][j] == 1) {   // only draw walls if m[i][j] != 0) {
				glPushMatrix();
				glTranslatef(i * 2.0, j * 2.0, 0.0);
				glScalef(0.8, 0.8, 0.8);
				colorcube();
				glPopMatrix();
			}
		}

	glPopMatrix();
}


void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	gluLookAt(viewer[0], viewer[1], viewer[2], 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);

	// PACMAN - MAP
	drawRawMap(map);
	eixos();

	glFlush();
		
	glutSwapBuffers();
}


void teclado(char key, int x, int y) {

	if (key == 'x') viewer[0] -= 1.0;
	if (key == 'X') viewer[0] += 1.0;
	if (key == 'y') viewer[1] -= 1.0;
	if (key == 'Y') viewer[1] += 1.0;
	if (key == 'z') viewer[2] -= 1.0;
	if (key == 'Z') viewer[2] += 1.0;

	glutPostRedisplay();
}


void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glFrustum(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w, 2.0 * (GLfloat)h / (GLfloat)w, 2.0, 20.0);
	else
		glFrustum(-2.0 * (GLfloat)w / (GLfloat)h, 2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, 2.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
}


void main(int argc, char** argv)
{
	glutInit(&argc, argv);

	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(500, 500);
	glutCreateWindow("Pac-Man");
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclado);
	glEnable(GL_DEPTH_TEST); /* Enable hidden--surface--removal */
	
	
	// PACMAN - MAP
	if (argc <= 1) {        // checking if the user give the map file
		printf("Reading default map %s...\n", "pm-simple.txt");
		openMapsFile("pm-simple.txt");       // Opem map file 
	}
	else {
		printf("Reading map %s...\n", argv[1]);
		openMapsFile(argv[1]);       // Opem map file 
	}

	map = nextMap();             // read the first map from file
	printMap();                  // print the map to the console

	
	glutMainLoop();
}
