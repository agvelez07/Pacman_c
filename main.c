#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pm-maps.h"
#include "character.h"
#include <math.h>

static GLfloat vertices[][3] = {
    {-1.0,-1.0,-0.1},{ 1.0,-1.0,-0.1},
    { 1.0, 1.0,-0.1},{-1.0, 1.0,-0.1},
    {-1.0,-1.0, 0.1},{ 1.0,-1.0, 0.1},
    { 1.0, 1.0, 0.1},{-1.0, 1.0, 0.1}
};

static GLfloat normals[][3] = {
    {-1.0,-1.0,-1.0},{ 1.0,-1.0,-1.0},
    { 1.0, 1.0,-1.0},{-1.0, 1.0,-1.0},
    {-1.0,-1.0, 1.0},{ 1.0,-1.0, 1.0},
    { 1.0, 1.0, 1.0},{-1.0, 1.0, 1.0}
};

static GLfloat colors[][3] = {
    {0.0,0.0,0.0},{1.0,0.0,0.0},
    {1.0,1.0,0.0},{0.0,1.0,0.0},
    {0.0,0.0,1.0},{1.0,0.0,1.0},
    {1.0,1.0,1.0},{0.0,1.0,1.0}
};

static char** map = NULL;

static double camX = 0.0;
static double camY = 0.0;
static double camZ = 12.0;

double centerX;
double centerY;

double maxX;
double maxY;

static void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glNormal3fv(normals[a]); glVertex3fv(vertices[a]);
    glNormal3fv(normals[b]); glVertex3fv(vertices[b]);
    glNormal3fv(normals[c]); glVertex3fv(vertices[c]);
    glNormal3fv(normals[d]); glVertex3fv(vertices[d]);
    glEnd();
}

static void colorcube(void)
{
    glColor3fv(colors[1]); polygon(0, 3, 2, 1);
    glColor3fv(colors[2]); polygon(2, 3, 7, 6);
    glColor3fv(colors[3]); polygon(0, 4, 7, 3);
    glColor3fv(colors[4]); polygon(1, 2, 6, 5);
    glColor3fv(colors[5]); polygon(4, 5, 6, 7);
    glColor3fv(colors[7]); polygon(0, 1, 5, 4);
}

void drawRawMap(char** m)
{
    int i, j;
    int cols = xTabSize();
    int rows = yTabSize();
    int maxSize = cols > rows ? cols : rows;

    centerX = (cols - 1);
    centerY = (rows - 1);

    maxX = centerX * 2;
    maxY = centerY * 2;

    glPushMatrix();

    glScaled(1.0 / maxSize, 1.0 / maxSize, 1.0 / maxSize);
    glTranslated(-centerX, -centerY, 0.0);

    for (i = 0; i < cols; i++)
        for (j = 0; j < rows; j++)
            if (m[i][j] == 1 || m[i][j] == '1') {
                glPushMatrix();
                glTranslated(i * 2.0, j * 2.0, 0.0);
                glScaled(0.8, 0.8, 0.8);
                colorcube();
                glPopMatrix();
            }

    glPopMatrix();
}

static void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslated(0.0, 0.0, -camZ);
    glTranslated(-camX, -camY, 0.0);

    drawRawMap(map);
    drawCharacter();

    glutSwapBuffers();
}

static void teclado(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    double pan = 0.3;
    double zoom = 0.7;

    if (key == 'x') camX += pan;
    if (key == 'X') camX -= pan;

    if (key == 'y') camY += pan;
    if (key == 'Y') camY -= pan;

    if (key == 'Z') {
        camZ -= zoom;
        if (camZ < 2.0) camZ = 2.0;
    }
    if (key == 'z') camZ += zoom;

    if (key == 'r' || key == 'R') {
        camX = 0.0;
        camY = 0.0;
        camZ = 12.0;
    }

    if (key == 27) exit(0);

    glutPostRedisplay();
}

static void reshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)w / (double)h, 0.1, 200.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Pac-Man");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(teclado);

    glEnable(GL_DEPTH_TEST);

    if (argc <= 1)
        openMapsFile("pm-simple.txt");
    else
        openMapsFile(argv[1]);

    map = nextMap();
    initCharacter(map);

    glutMainLoop();
    return 0;
}