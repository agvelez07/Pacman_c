#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct{
  int r;
  int c;
  int wallOn;
  int visited;
  int ocuppied;
}cellStatus;
