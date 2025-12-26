#ifndef CUBE_H
#define CUBE_H

typedef struct cube* Cube;
typedef struct index* Index;

Cube createCube();

void destroyCube(Cube cube);
void initCube(Cube cube, float x, float y, float z, float scaleX, float scaleY, float scaleZ);
void colorcube(void);
void drawCube(Cube cube);
void axes(void);

#endif