#ifndef BOARDH
#define BOARDH

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

void boardInit(char** map);
void boardDisplay(void);
void boardKey(unsigned char key, int x, int y);
void boardReshape(int w, int h);

#endif
