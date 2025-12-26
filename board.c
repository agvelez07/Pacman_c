#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "board.h"
#include "cube.h"
#include "pm-maps.h"
#include "character.h"

#include <stdlib.h>

#define W 500
#define H 500

static char** mapRef = NULL;

static GLfloat camX = 0.0;
static GLfloat camY = 0.0;
static GLfloat camZ = 12.0;

static GLfloat centerX = 0.0;
static GLfloat centerY = 0.0;
static GLfloat maxX = 0.0;
static GLfloat maxY = 0.0;
static GLfloat minX = 0.0;
static GLfloat minY = 0.0;

static void drawRawMap(char** map)
{
    int cols = xTabSize();
    int rows = yTabSize();
    int maxSize = cols > rows ? cols : rows;

    if (!map || cols <= 0 || rows <= 0) return;

    centerX = (GLfloat)(cols - 1);
    centerY = (GLfloat)(rows - 1);

    maxX = centerX + (GLfloat)(W / 2);
    maxY = centerY + (GLfloat)(H / 2);

    minX = centerX - (GLfloat)W;
    minY = centerY - (GLfloat)H;

    glPushMatrix();

    glScalef(1.0 / (GLfloat)maxSize, 1.0 / (GLfloat)maxSize, 1.0 / (GLfloat)maxSize);
    glTranslatef(-centerX, -centerY, 0.0);

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            if (map[c][r] == '1' || map[c][r] == 1) {
                glPushMatrix();
                glTranslatef((GLfloat)(c * 2), (GLfloat)(r * 2), 0.0);
                glScalef(0.8, 0.8, 0.8);
                colorCube();
                glPopMatrix();
            }
        }
    }

    glPopMatrix();
}

void boardInit(char** map)
{
    mapRef = map;
    camX = 0.0;
    camY = 0.0;
    camZ = 12.0;
}

void boardDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, -camZ);
    glTranslatef(-camX, -camY, 0.0);

    drawRawMap(mapRef);
    characterDraw();

    glutSwapBuffers();
}

void boardKey(unsigned char key, int x, int y)
{
    (void)x;
    (void)y;

    const GLfloat pan = 0.3;
    const GLfloat zoom = 0.7;

    if (key == 'x' && camX <= maxX) camX += pan;
    if (key == 'X' && camX >= minX) camX -= pan;

    if (key == 'y') camY += pan;
    if (key == 'Y') camY -= pan;

    if (key == 'Z') { camZ -= zoom; if (camZ < 2.0) camZ = 2.0; }
    if (key == 'z') camZ += zoom;

    if (key == 'r' || key == 'R') {
        camX = 0.0;
        camY = 0.0;
        camZ = 12.0;
    }

    if (key == 'w' || key == 'W' ||
        key == 'a' || key == 'A' ||
        key == 's' || key == 'S' ||
        key == 'd' || key == 'D')
    {
        characterMove(key);
    }

    if (key == 27) exit(0);

    glutPostRedisplay();
}

void boardReshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
}
