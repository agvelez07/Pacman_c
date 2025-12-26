#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "character.h"
#include "pm-maps.h"

#include <stdlib.h>
#include <time.h>

struct pacman {
    int c;
    int r;
    int alive;
};

static char** mapRef = NULL;

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

static int collectHouses(char** map, HousePos* houses)
{
    int count = 0;
    int cols = xTabSize();
    int rows = yTabSize();

    for (int c = 0; c < cols; c++) {
        for (int r = 0; r < rows; r++) {
            if (map[c][r] == '1' || map[c][r] == 1) {
                houses[count].c = c;
                houses[count].r = r;
                count++;
            }
        }
    }
    return count;
}

void characterInit(char** map)
{
    mapRef = map;
    if (!mapRef) return;

    static int seeded = 0;
    if (!seeded) { srand((unsigned)time(NULL)); seeded = 1; }

    int cols = xTabSize();
    int rows = yTabSize();
    int max = cols * rows;
    if (max <= 0) return;

    HousePos* houses = (HousePos*)malloc((size_t)max * sizeof(HousePos));
    if (!houses) return;

    int count = collectHouses(mapRef, houses);
    if (count > 0) {
        int k = rand() % count;
        pac->c = houses[k].c;
        pac->r = houses[k].r;
        pac->alive = 1;
    }

    free(houses);
}

void characterDraw(void)
{
    int cols = xTabSize();
    int rows = yTabSize();
    int maxSize = cols > rows ? cols : rows;
    if (maxSize <= 0) return;

    GLfloat s = 1.0 / (GLfloat)maxSize;

    glPushMatrix();

    glScalef(s, s, s);
    glTranslatef(-(GLfloat)(cols - 1), -(GLfloat)(rows - 1), 0.0);

    glTranslatef((GLfloat)(pac->c * 2), (GLfloat)(pac->r * 2), 0.5);
    glScalef(0.6f, 0.6f, 0.6f);

    drawPacman();

    glPopMatrix();
}

int characterMove(unsigned char key)
{
    if (!mapRef) return 0;
    if (!pac || pac->alive != 1) return 0;

    int dc = 0;
    int dr = 0;

    if (key == 'w' || key == 'W') dr = 1;
    if (key == 's' || key == 'S') dr = -1;
    if (key == 'd' || key == 'D') dc = 1;
    if (key == 'a' || key == 'A') dc = -1;

    if (dc == 0 && dr == 0) return 0;

    int nc = pac->c + dc;
    int nr = pac->r + dr;

    if (nc < 0 || nc >= xTabSize()) return 0;
    if (nr < 0 || nr >= yTabSize()) return 0;

    if (!(mapRef[nc][nr] == '1' || mapRef[nc][nr] == 1)) return 0;

    pac->c = nc;
    pac->r = nr;
    return 1;
}

Pacman characterGet(void)
{
    return pac;
}
