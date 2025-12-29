#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "character.h"
#include "pm-maps.h"
#include "board.h"

#include <stdlib.h>
#include <time.h>

struct pacman {
    int c;
    int r;
    int alive;
};

static struct pacman pacData = { 0, 0, 0 };
static Pacman pac = &pacData;

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

typedef struct {
    int c;
    int r;
} HousePos;

static int collectHouses(Map m, HousePos* houses, int max)
{
    int count = 0;
    int cols = mapXSize(m);
    int rows = mapYSize(m);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            Cell cell = cellAt(m, r, c);
            if (!cell) continue;
            if (!cellIsWall(cell)) continue;
            if (count < max) {
                houses[count].c = c;
                houses[count].r = r;
                count++;
            }
        }
    }

    return count;
}

int getPacmanRow(Pacman p)
{
    return p ? p->r : -1;
}

int getPacmanCol(Pacman p)
{
    return p ? p->c : -1;
}

void characterInit(void)
{
    Map m = getCurrentMap();
    if (!m) return;

    static int seeded = 0;
    if (!seeded) {
        srand((unsigned)time(NULL));
        seeded = 1;
    }

    int cols = mapXSize(m);
    int rows = mapYSize(m);
    int max = cols * rows;
    if (max <= 0) return;

    HousePos* houses = (HousePos*)malloc((size_t)max * sizeof(HousePos));
    if (!houses) return;

    int count = collectHouses(m, houses, max);
    if (count > 0) {
        int k = rand() % count;
        pac->c = houses[k].c;
        pac->r = houses[k].r;
        pac->alive = 1;
        setBoardPacman(pac);
    }

    free(houses);
}

void characterDraw(void)
{
    Map m = getCurrentMap();
    if (!m) return;

    Pacman p = getPacman();
    if (!p || p->alive != 1) return;

    int cols = mapXSize(m);
    int rows = mapYSize(m);
    int maxSize = cols > rows ? cols : rows;
    if (maxSize <= 0) return;

    GLfloat s = 1.0 / (GLfloat)maxSize;

    int rr = (rows - 1) - p->r;

    glPushMatrix();

    glScalef(s, s, s);
    glTranslatef(-(GLfloat)(cols - 1), -(GLfloat)(rows - 1), 0.0);

    glTranslatef((GLfloat)(p->c * 2), (GLfloat)(rr * 2), 0.6);
    glScalef(0.6, 0.6, 0.6);

    drawPacman();

    glPopMatrix();
}

int characterMove(unsigned char key)
{
    Map m = getCurrentMap();
    if (!m) return 0;

    Pacman p = getPacman();
    if (!p || p->alive != 1) return 0;

    int dc = 0;
    int dr = 0;

    if (key == 'w' || key == 'W') dr = -1;
    if (key == 's' || key == 'S') dr = 1;
    if (key == 'd' || key == 'D') dc = 1;
    if (key == 'a' || key == 'A') dc = -1;

    if (dc == 0 && dr == 0) return 0;

    int nc = p->c + dc;
    int nr = p->r + dr;

    Cell next = cellAt(m, nr, nc);
    if (!next) return 0;
    if (!cellIsWall(next)) return 0;

    p->c = nc;
    p->r = nr;

    return 1;
}
