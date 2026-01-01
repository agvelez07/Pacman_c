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
#include <stdio.h>
#include <string.h>
#include <math.h>

#define PI 3.14159
#define STACK_MAX  64
#define SECTOR_MAX 64
#define VERT_MAX ((STACK_MAX + 1) * (SECTOR_MAX + 1))

struct pacman {
    int c;
    int r;
    int nC;
    int nR;
    int alive;
    char currentKey;
    char nextKey;
    float t;
    int moving;
};

struct ghost {
    int c;
    int r;
    int nC;
    int nR;
    float t;
    int moving;
    float speed;
    float colorR, colorG, colorB;
};

typedef struct {
    int c;
    int r;
} HousePos;

float mouthAngleDeg = 10.0;
int mouthOpening = 1;

static GLfloat sphereVertices[VERT_MAX][3];
static GLfloat sphereNormals[VERT_MAX][3];
static GLfloat sphereTex[VERT_MAX][2];

float radius = 0.48;
int stackCount = 32;
int sectorCount = 48;

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

void updateMouth(void)
{
    if (mouthOpening) mouthAngleDeg += 1.0;
    else mouthAngleDeg -= 1.0;

    if (mouthAngleDeg > 40.0) mouthOpening = 0;
    if (mouthAngleDeg < 5.0)  mouthOpening = 1;
}

static int isGhostAt(Ghost* ghosts, int count, int r, int c, int skipIndex)
{
    for (int i = 0; i < count; i++) {
        if (i == skipIndex) continue;
        if (ghosts[i]->r == r && ghosts[i]->c == c) return 1;
    }
    return 0;
}

static int tryMoveGhost(Ghost g, int dc, int dr, Map m, Ghost* allGhosts, int ghostCount, int myIndex)
{
    int nextC = g->c + dc;
    int nextR = g->r + dr;

    Cell cell = cellAt(m, nextR, nextC);
    if (!cell || !cellIsWall(cell)) return 0;

    if (isGhostAt(allGhosts, ghostCount, nextR, nextC, myIndex)) return 0;

    g->nC = nextC;
    g->nR = nextR;
    g->t = 0.0;
    g->moving = 1;
    return 1;
}

static void moveGhostRandom(Ghost g, Map m, Ghost* allGhosts, int ghostCount, int myIndex)
{
    int directions[4][2] = { {0, -1}, {0, 1}, {-1, 0}, {1, 0} };

    for (int i = 3; i > 0; i--) {
        int j = rand() % (i + 1);
        int tempC = directions[i][0];
        int tempR = directions[i][1];
        directions[i][0] = directions[j][0];
        directions[i][1] = directions[j][1];
        directions[j][0] = tempC;
        directions[j][1] = tempR;
    }

    for (int d = 0; d < 4; d++) {
        if (tryMoveGhost(g, directions[d][0], directions[d][1], m, allGhosts, ghostCount, myIndex)) {
            return;
        }
    }
}

void updateAllGhosts(void)
{
    Ghost* ghosts = getGhosts();
    if (!ghosts) return;

    int ghostCount = getBoardGhostCount();
    if (ghostCount <= 0) return;

    Pacman p = getPacman();
    if (!p) return;

    Map m = getCurrentMap();
    if (!m) return;

    for (int i = 0; i < ghostCount; i++) {
        Ghost g = ghosts[i];
        if (!g) continue;

        if (g->moving) {
            g->t += g->speed;

            if (g->t >= 1.0) {
                g->t = 1.0;
                g->c = g->nC;
                g->r = g->nR;
                g->moving = 0;
                g->t = 0.0;
            }
        }
        else {
            int deltaC = p->c - g->c;
            int deltaR = p->r - g->r;

            int moved = 0;

            if (abs(deltaC) >= abs(deltaR)) {
                if (deltaC != 0) {
                    int dc = (deltaC > 0) ? 1 : -1;
                    if (tryMoveGhost(g, dc, 0, m, ghosts, ghostCount, i)) {
                        moved = 1;
                    }
                }

                if (!moved && deltaR != 0) {
                    int dr = (deltaR > 0) ? 1 : -1;
                    if (tryMoveGhost(g, 0, dr, m, ghosts, ghostCount, i)) {
                        moved = 1;
                    }
                }
            }
            else {
                if (deltaR != 0) {
                    int dr = (deltaR > 0) ? 1 : -1;
                    if (tryMoveGhost(g, 0, dr, m, ghosts, ghostCount, i)) {
                        moved = 1;
                    }
                }

                if (!moved && deltaC != 0) {
                    int dc = (deltaC > 0) ? 1 : -1;
                    if (tryMoveGhost(g, dc, 0, m, ghosts, ghostCount, i)) {
                        moved = 1;
                    }
                }
            }

            if (!moved) {
                moveGhostRandom(g, m, ghosts, ghostCount, i);
            }
        }
    }
}

void timer(int v)
{
    updateMouth();

    Pacman p = getPacman();
    if (p && p->alive == 1 && p->moving) {
        p->t += 0.05;
        if (p->t >= 1.0) {
            p->t = 1.0;
            p->c = p->nC;
            p->r = p->nR;
            p->moving = 0;
            p->t = 0.0;
        }
    }

    updateAllGhosts();

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

static void drawEye(void)
{
    glColor3f(0.0, 0.0, 0.0);
    glutSolidSphere(0.08, 16, 16);
}

int setPacmanNextKey(char key)
{
    Pacman p = getPacman();
    if (!p) return 0;
    p->nextKey = key;
    return 1;
}

int pacmanChangedKey(void)
{
    Pacman p = getPacman();
    if (!p) return -1;
    if (p->currentKey != p->nextKey) return 1;
    return 0;
}

static void drawPacman(void)
{
    glColor3f(1.0, 1.0, 0.0);

    int cut = (int)((mouthAngleDeg / 360.0) * (float)sectorCount);
    if (cut < 0) cut = 0;
    if (cut > sectorCount / 2) cut = sectorCount / 2;

    for (int i = 0; i < stackCount; ++i)
    {
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= sectorCount; ++j)
        {
            if (j <= cut || j >= (sectorCount - cut)) continue;

            int k1 = i * (sectorCount + 1) + j;
            int k2 = (i + 1) * (sectorCount + 1) + j;

            if (k2 >= VERT_MAX) continue;

            glNormal3fv(sphereNormals[k1]);
            glVertex3fv(sphereVertices[k1]);

            glNormal3fv(sphereNormals[k2]);
            glVertex3fv(sphereVertices[k2]);
        }
        glEnd();
    }

    glPushMatrix();
    glTranslatef(0.15, 0.40, -0.15);
    drawEye();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15, 0.40, 0.15);
    drawEye();
    glPopMatrix();
}

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

void buildSphere(void)
{
    if (stackCount > STACK_MAX)  stackCount = STACK_MAX;
    if (sectorCount > SECTOR_MAX) sectorCount = SECTOR_MAX;

    float lengthInv = 1.0 / radius;
    float sectorStep = 2.0 * PI / (float)sectorCount;
    float stackStep = PI / (float)stackCount;

    for (int i = 0; i <= stackCount; ++i)
    {
        float stackAngle = (PI * 0.5) - (float)i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);

        for (int j = 0; j <= sectorCount; ++j)
        {
            float sectorAngle = (float)j * sectorStep;

            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);

            int idx = i * (sectorCount + 1) + j;

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

void characterInit(void)
{
    static struct pacman pacData = { 0, 0, 0, 0, 0, 0, 0, 0.0, 0 };
    static Pacman pac = &pacData;

    static int sphereBuilt = 0;
    if (!sphereBuilt) {
        buildSphere();
        sphereBuilt = 1;
    }

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
        pac->nC = pac->c;
        pac->nR = pac->r;
        pac->t = 0.0;
        pac->moving = 0;
        pac->nextKey = 0;
        pac->currentKey = 0;
        setBoardPacman(pac);
    }

    int ghostCount = getBoardGhostCount();
    if (ghostCount > 0) {

        Ghost* ghostArray = (Ghost*)malloc((size_t)ghostCount * sizeof(Ghost));
        if (!ghostArray) { free(houses); return; }

        for (int i = 0; i < ghostCount; i++) ghostArray[i] = NULL;

        for (int i = 0; i < ghostCount; i++) {

            ghostArray[i] = (Ghost)malloc(sizeof(struct ghost));
            if (!ghostArray[i]) {
                for (int j = 0; j < i; j++) free(ghostArray[j]);
                free(ghostArray);
                free(houses);
                return;
            }

            HousePos* housesGhost = (HousePos*)malloc((size_t)max * sizeof(HousePos));
            if (!housesGhost) {
                for (int j = 0; j <= i; j++) free(ghostArray[j]);
                free(ghostArray);
                free(houses);
                return;
            }

            int countG = collectHouses(m, housesGhost, max);
            if (countG > 0) {
                int k = rand() % countG;

                ghostArray[i]->c = housesGhost[k].c;
                ghostArray[i]->r = housesGhost[k].r;
                ghostArray[i]->nC = ghostArray[i]->c;
                ghostArray[i]->nR = ghostArray[i]->r;

                ghostArray[i]->t = 0.0;
                ghostArray[i]->moving = 0;
                ghostArray[i]->speed = 0.01 + ((float)(rand() % 4) * 0.01);

                ghostArray[i]->colorR = 0.1 + ((float)rand() / (float)RAND_MAX) * 0.9;
                ghostArray[i]->colorG = 0.1 + ((float)rand() / (float)RAND_MAX) * 0.9;
                ghostArray[i]->colorB = 0.1 + ((float)rand() / (float)RAND_MAX) * 0.9;
            }
            else {
                ghostArray[i]->c = 0;
                ghostArray[i]->r = 0;
                ghostArray[i]->nC = ghostArray[i]->c;
                ghostArray[i]->nR = ghostArray[i]->r;
                ghostArray[i]->t = 0.0;
                ghostArray[i]->moving = 0;
                ghostArray[i]->speed = 0.03;
                ghostArray[i]->colorR = 1.0;
                ghostArray[i]->colorG = 0.0;
                ghostArray[i]->colorB = 1.0;
            }

            free(housesGhost);
        }

        setBoardGhosts(ghostArray, ghostCount);
    }

    free(houses);
}

static void drawSingleGhost(Ghost g)
{
    glColor3f(g->colorR, g->colorG, g->colorB);
    glutSolidCube(1.0);

    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glTranslatef(-0.2, 0.2, 0.55);
    glutSolidSphere(0.15, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 0.2, 0.55);
    glutSolidSphere(0.15, 16, 16);
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-0.2, 0.2, 0.65);
    glutSolidSphere(0.08, 16, 16);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.2, 0.2, 0.65);
    glutSolidSphere(0.08, 16, 16);
    glPopMatrix();
}

void ghostsDraw(Ghost* ghosts)
{
    if (!ghosts) return;

    Map m = getCurrentMap();
    if (!m) return;

    int cols = mapXSize(m);
    int rows = mapYSize(m);
    int maxSize = cols > rows ? cols : rows;
    if (maxSize <= 0) return;

    int ghostsCount = getBoardGhostCount();
    if (ghostsCount <= 0) return;

    GLfloat s = 1.0 / (GLfloat)maxSize;

    for (int i = 0; i < ghostsCount; i++) {
        Ghost g = ghosts[i];
        if (!g) continue;

        float drawC = (float)g->c;
        float drawR = (float)g->r;

        if (g->moving) {
            drawC = (float)g->c + g->t * ((float)g->nC - (float)g->c);
            drawR = (float)g->r + g->t * ((float)g->nR - (float)g->r);
        }

        float rr = (float)(rows - 1) - drawR;

        glPushMatrix();

        glScalef(s, s, s);
        glTranslatef(-(GLfloat)(cols - 1), -(GLfloat)(rows - 1), 0.0);

        glTranslatef((GLfloat)(drawC * 2), (GLfloat)(rr * 2), 1.0);
        glScalef(1.6, 1.6, 1.6);

        glRotatef(90.0, 1.0, 0.0, 0.0);

        drawSingleGhost(g);

        glPopMatrix();
    }
}

int isPacmanAlive(void)
{
    Pacman p = getPacman();
    if (!p) return 0;
    return p->alive;
}

int pacmanDie(void)
{
    Pacman p = getPacman();
    if (!p) return 0;
    p->alive = 0;
    return 1;
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

    float drawC = (float)p->c;
    float drawR = (float)p->r;

    if (p->moving) {
        drawC = (float)p->c + p->t * ((float)p->nC - (float)p->c);
        drawR = (float)p->r + p->t * ((float)p->nR - (float)p->r);
    }

    float rr = (float)(rows - 1) - drawR;

    glPushMatrix();

    glScalef(s, s, s);
    glTranslatef(-(GLfloat)(cols - 1), -(GLfloat)(rows - 1), 0.0);

    glTranslatef((GLfloat)(drawC * 2.0), (GLfloat)(rr * 2.0), 1.0);
    glScalef(1.6, 1.6, 1.6);

    glRotatef(90.0, 1.0, 0.0, 0.0);

    switch (p->currentKey) {
    case 'd':
        break;

    case 'w':
        glRotatef(90.0, 0.0, 1.0, 0.0);
        break;

    case 'a':
        glRotatef(180.0, 0.0, 1.0, 0.0);
        break;

    case 's':
        glRotatef(270.0, 0.0, 1.0, 0.0);
        break;
    }

    drawPacman();

    glPopMatrix();
}

int setPacmanCurrentKey(void)
{
    Pacman p = getPacman();
    if (!p) return 0;
    if (!p->nextKey) return 0;
    p->currentKey = p->nextKey;
    return 1;
}

int characterMove(unsigned char key)
{
    Map m = getCurrentMap();
    if (!m) return 0;

    Pacman p = getPacman();
    if (!p || p->alive != 1) return 0;

    if (p->moving) return 0;

    int dc = 0;
    int dr = 0;

    if (key == 'w' || key == 'W') {
        dr = -1;
        if (key != p->currentKey) {
            setPacmanNextKey('w');
            setPacmanCurrentKey();
        }
    }
    if (key == 's' || key == 'S') {
        dr = 1;
        if (key != p->currentKey) {
            setPacmanNextKey('s');
            setPacmanCurrentKey();
        }
    }
    if (key == 'd' || key == 'D') {
        dc = 1;
        if (key != p->currentKey) {
            setPacmanNextKey('d');
            setPacmanCurrentKey();
        }
    }
    if (key == 'a' || key == 'A') {
        dc = -1;
        if (key != p->currentKey) {
            setPacmanNextKey('a');
            setPacmanCurrentKey();
        }
    }

    if (dc == 0 && dr == 0) return 0;

    int nc = p->c + dc;
    int nr = p->r + dr;

    Cell next = cellAt(m, nr, nc);
    if (!next) return 0;
    if (!cellIsWall(next)) return 0;

    p->nC = nc;
    p->nR = nr;
    p->t = 0.0;
    p->moving = 1;

    return 1;
}