\
#include "cube.h"

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

typedef struct cube* Cube;

typedef struct Index {
    int r;
    int c; 
};

extern int xTabSize(void); 
extern int yTabSize(void);


static Index* houses = NULL;
static int housesCount = 0;

static void gridToWorld(int r, int c, float cellSize, float* x, float* z)
{
    int rows = yTabSize();
    int cols = xTabSize();

    *x = (c - cols / 2.0f) * cellSize + cellSize / 2.0f;


    *z = (rows / 2.0f - r) * cellSize - cellSize / 2.0f;
}


void getHousesIndex(char** m)
{
    int r, c;
    int rows = yTabSize();
    int cols = xTabSize();

    housesCount = 0;
    for (r = 0; r < rows; r++) {
        for (c = 0; c < cols; c++) {
            if (m[r][c] == '1') { 
                housesCount++;
            }
        }
    }


    free(houses);
    houses = NULL;

    if (housesCount <= 0) {
        return;
    }

    houses = (Index*)malloc(sizeof(Index) * housesCount);
    if (!houses) {
        housesCount = 0;
        return;
    }

    int k = 0;
    for (r = 0; r < rows; r++) {
        for (c = 0; c < cols; c++) {
            if (m[r][c] == '1') { 
                houses[k++] = (Index){ r, c };
            }
        }
    }
}

Index getRandomHouse(void)
{
    if (housesCount <= 0) return (Index) { 0, 0 };
    int i = rand() % housesCount;
    return houses[i];
}

Cube createCubeC(Index I)
{
    Cube cube = (Cube)malloc(sizeof(cube));
    if (!cube) return NULL;

    float x, z;
    gridToWorld(I.r, I.c, 1.0, &x, &z);

    cube->x = x;
    cube->y = 0.0f;
    cube->z = z;

    cube->scaleX = 1.0f;
    cube->scaleY = 1.0f;
    cube->scaleZ = 1.0f;

    cube->theta[0] = 0.0f;
    cube->theta[1] = 0.0f;
    cube->theta[2] = 0.0f;

    return cube;
}

Cube startCharachter(char** m)
{
    getHousesIndex(m);

    Index pos = getRandomHouse();
    Cube player = createCubeC(pos);
    if (!player) return NULL;

    return player;
}

void freeHouses(void)
{
    free(houses);
    houses = NULL;
    housesCount = 0;
}
