#include <stdlib.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "pm-maps.h"
#include "board.h"
#include "character.h"

#define W 500
#define H 500

static char** mapRef = NULL;

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(W, H);
    glutCreateWindow("Pac-Man");

    glEnable(GL_DEPTH_TEST);

    boardInit(argc <= 1 ? "pm-simple.txt" : argv[1]);

    glutDisplayFunc(boardDisplay);
    glutReshapeFunc(boardReshape);
    glutKeyboardFunc(boardKey);

    glutMainLoop();
    return 0;
}