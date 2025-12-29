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
#include <math.h>

#define PI 3.14159f
#define STACK_MAX  64
#define SECTOR_MAX 64
#define VERT_MAX ((STACK_MAX + 1) * (SECTOR_MAX + 1))

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


/*
 *Implementação Esfera, com base na documentação:
 * https://www.songho.ca/opengl/gl_sphere.html 
 */
static GLfloat sphereVertices[VERT_MAX][3];
static GLfloat sphereNormals [VERT_MAX][3];
static GLfloat sphereTex     [VERT_MAX][2];

float radius = 0.48;
int stackCount = 32;
int sectorCount = 48;


void buildSphere(void)
{
    if (stackCount > STACK_MAX)  stackCount = STACK_MAX;
    if (sectorCount > SECTOR_MAX) sectorCount = SECTOR_MAX;

    float lengthInv = 1.0 / radius;
    float sectorStep = 2.0 * PI / (float)sectorCount;
    float stackStep  = PI / (float)stackCount;

    for (int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = (PI * 0.5) - (float)i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z  = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = (float)j * sectorStep;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            int idx = i * (sectorCount + 1) + j;   /* usa o count real */

            sphereVertices[idx][0] = x;
            sphereVertices[idx][1] = y;
            sphereVertices[idx][2] = z;

            sphereNormals[idx][0] = x * lengthInv;
            sphereNormals[idx][1] = y * lengthInv;
            sphereNormals[idx][2] = z * lengthInv;

            sphereTex[idx][0] = (float)j / (float)sectorCount;
            sphereTex[idx][1] = (float)i / (float)stackCount;
        }
    }
}
