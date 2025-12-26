#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "cube.h"

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

static void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glNormal3fv(normals[a]); glVertex3fv(vertices[a]);
    glNormal3fv(normals[b]); glVertex3fv(vertices[b]);
    glNormal3fv(normals[c]); glVertex3fv(vertices[c]);
    glNormal3fv(normals[d]); glVertex3fv(vertices[d]);
    glEnd();
}

void colorCube(void)
{
    glColor3fv(colors[1]); polygon(0, 3, 2, 1);
    glColor3fv(colors[2]); polygon(2, 3, 7, 6);
    glColor3fv(colors[3]); polygon(0, 4, 7, 3);
    glColor3fv(colors[4]); polygon(1, 2, 6, 5);
    glColor3fv(colors[5]); polygon(4, 5, 6, 7);
    glColor3fv(colors[7]); polygon(0, 1, 5, 4);
}
