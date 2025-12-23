#include "character.h"
#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>

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

static Pacman gPac = {0, 0};

typedef struct { int r, c; } HousePos;

static void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
        glNormal3fv(normals[a]); glVertex3fv(vertices[a]);
        glNormal3fv(normals[b]); glVertex3fv(vertices[b]);
        glNormal3fv(normals[c]); glVertex3fv(vertices[c]);
        glNormal3fv(normals[d]); glVertex3fv(vertices[d]);
    glEnd();
}

static void drawPacman(void)
{
    glColor3f(1.0, 1.0, 0.0); 
    polygon(0, 3, 2, 1);
    polygon(2, 3, 7, 6);
    polygon(0, 4, 7, 3);
    polygon(1, 2, 6, 5);
    polygon(4, 5, 6, 7);
    polygon(0, 1, 5, 4);
}

static int getAvailableHouses(char** map, HousePos* houses)
{
    int count = 0;
    for (int c = 0; c < xTabSize(); c++) {
        for (int r = 0; r < yTabSize(); r++) {
            if (map[c][r] == 1) {
                houses[count].c = c;
                houses[count].r = r;
                count++;
            }
        }
    }
    return count;
}

void initCharacter(char** map)
{
    if (!map) return;

    int max = xTabSize() * yTabSize();
    if (max <= 0) return;

    HousePos* houses = (HousePos*)malloc(sizeof(HousePos) * max);
    if (!houses) return;

    int count = getAvailableHouses(map, houses);
    if (count > 0) {
        int k = rand() % count;
        gPac.c = houses[k].c;
        gPac.r = houses[k].r;
        printf("Pacman spawn: c=%d r=%d\n", gPac.c, gPac.r);
    }

    free(houses);
}

void drawCharacter(void)
{
    int maxSize = xTabSize() > yTabSize() ? xTabSize() : yTabSize();
    if (maxSize <= 0) return;

    float S = 1.0 / (float)maxSize;

    glPushMatrix();

        glScalef(S, S, S);
        glTranslatef(-(xTabSize() - 1), -(yTabSize() - 1), 0.0);

        glTranslatef(gPac.c * 2.0, gPac.r * 2.0, 0.25);
        glScalef(0.6, 0.6, 0.6);

        drawPacman();

    glPopMatrix();
}

Pacman getCharacterPos(void)
{
    return gPac;
}

int moveCharacter(char** map, int dc, int dr)
{
    if (!map) return 0;

    int nc = gPac.c + dc;
    int nr = gPac.r + dr;

    if (nc < 0 || nc >= xTabSize()) return 0;
    if (nr < 0 || nr >= yTabSize()) return 0;

    if (map[nc][nr] != 1) return 0;

    gPac.c = nc;
    gPac.r = nr;
    return 1;
}
