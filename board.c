#ifdef __APPLE__
#include <GLUT



glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>

#include "board.h"
#include "cube.h"
#include "pm-maps.h"
#include "character.h"

#define W 500
#define H 500

#define PI 3.14159

struct board {
    Map* maps;
    int mapsCount;
    int currentMap;
    int wallMode;
    int gameStatus;
    int ghostCount;
    Pacman pacman;
	Ghost* ghosts;
};

static GLfloat camX = 0.0;
static GLfloat camY = 0.0;
static GLfloat camZ = 3.0;

static GLfloat camPitch = 0.0;  
static GLfloat camYaw = 0.0;   

static GLfloat centerX = 0.0;
static GLfloat centerY = 0.0;
static GLfloat maxX = 0.0;
static GLfloat maxY = 0.0;
static GLfloat minX = 0.0;
static GLfloat minY = 0.0;

static Board gBoard = NULL;

Board getBoard(void)
{
    return gBoard;
}

Map getCurrentMap(void)
{
    if (gBoard && gBoard->mapsCount > 0)
        return gBoard->maps[gBoard->currentMap];
    return NULL;
}

int getBoardMapCount(void)
{
    return gBoard ? gBoard->mapsCount : 0;
}

int getCurrentMapIndex(void)
{
    return (gBoard && gBoard->mapsCount > 0) ? gBoard->currentMap : -1;
}

Pacman getPacman(void)
{
    return gBoard ? gBoard->pacman : NULL;
}

Ghost* getGhosts(void)
{
    return gBoard ? gBoard->ghosts : NULL;
}

int setBoardPacman(Pacman p)
{
    if (!gBoard) return 0;
    gBoard->pacman = p;
    return 1;
}

int setBoardGhostCount(int ghostsCount)
{
    if (!gBoard) return 0;
    gBoard->ghostCount = ghostsCount;
    return 1;
}

void ClearGhosts() {
    if (!gBoard || !gBoard->ghosts) return;
    int ghostCount = getBoardGhostCount();
    if (ghostCount <= 0) return;
    for (int i = 0; i < ghostCount; i++) {
        if (gBoard->ghosts[i]) {
            free(gBoard->ghosts[i]);
            gBoard->ghosts[i] = NULL;
        }
    }
    free(gBoard->ghosts);
    gBoard->ghosts = NULL;
}

int getBoardGhostCount(void)
{
    return gBoard ? gBoard->ghostCount : 0;
}

int setBoardGhosts(Ghost* ghosts, int ghostCount)
{
    if (!gBoard) return 0;

    ClearGhosts();


    if (!ghosts || ghostCount <= 0) {
        return 1; 
    }

    gBoard->ghosts = ghosts;
	gBoard->ghostCount = ghostCount;

    return 1;
}


static void drawMap(Map m)
{
    if (!m) return;

    int cols = mapXSize(m);
    int rows = mapYSize(m);
    int maxSize = cols > rows ? cols : rows;

    if (cols <= 0 || rows <= 0 || maxSize <= 0) return;

    centerX = (GLfloat)(cols - 1);
    centerY = (GLfloat)(rows - 1);

    maxX = centerX + (GLfloat)(W / 2);
    maxY = centerY + (GLfloat)(H / 2);

    minX = centerX - (GLfloat)W;
    minY = centerY - (GLfloat)H;

    glPushMatrix();

    glScalef(1.0 / (GLfloat)maxSize, 1.0 / (GLfloat)maxSize, 1.0 / (GLfloat)maxSize);
    glTranslatef(-centerX, -centerY, 0.0);

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {

            Cell cell = cellAt(m, r, c);
            if (!cell) continue;

            if (!cellIsWall(cell)) continue;

            glPushMatrix();
            int rr = (rows - 1) - r;
            glTranslatef((GLfloat)(c * 2), (GLfloat)(rr * 2), 0.0);
            glScalef(0.9, 0.9, 0.2);

            colorCube(cell);

            glPopMatrix();
        }
    }

    glPopMatrix();
}

static void boardFreeMaps(Board b)
{
    if (!b) return;

    for (int i = 0; i < b->mapsCount; i++)
        mapDestroy(b->maps[i]);

    free(b->maps);
    b->maps = NULL;
    b->mapsCount = 0;
}

void boardDestroy(void)
{
    if (!gBoard) return;

    boardFreeMaps(gBoard);

    free(gBoard);
    gBoard = NULL;
}

void boardInit(const char* mapFile)
{
    if (gBoard) return;

    gBoard = (Board)malloc(sizeof(struct board));
    if (!gBoard) exit(1);

    gBoard->maps = NULL;
    gBoard->mapsCount = 0;
    gBoard->currentMap = 0;
    gBoard->wallMode = 0;
    gBoard->gameStatus = 0;
    gBoard->pacman = NULL;
	gBoard->ghostCount = 0;
    gBoard->ghosts = NULL;

    gBoard->mapsCount = readAllMaps(&gBoard->maps, mapFile);
    if (gBoard->mapsCount <= 0) exit(1);

    gBoard->currentMap = 0;

    characterInit();
}

void boardDisplay(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (gBoard && gBoard->mapsCount > 0) {
        Map m = gBoard->maps[gBoard->currentMap];

        int cols = mapXSize(m);
        int rows = mapYSize(m);

        if (cols > 0 && rows > 0) {
            centerX = (GLfloat)(cols - 1);
            centerY = (GLfloat)(rows - 1);
        }

        glTranslatef(0.0, 0.0, -camZ);
         
        glRotatef(camPitch, 1.0, 0.0, 0.0);
         
        glRotatef(camYaw, 0.0, 0.0, 1.0);

        glTranslatef(-camX, -camY, 0.0);

        drawMap(m);
    }

    characterDraw();
    ghostsDraw(gBoard->ghosts);

    glutSwapBuffers();
}

void boardKey(unsigned char key, int x, int y) 
{
    (void)x;
    (void)y;

    const GLfloat pan = 0.3;
    const GLfloat zoom = 0.7;
    const GLfloat rotStep = 2.0;

    if (key == 'x' && camX <= maxX) camX += pan;
    if (key == 'X' && camX >= minX) camX -= pan;

    if (key == 'y') camY += pan;
    if (key == 'Y') camY -= pan;

    if (key == 'Z') { camZ -= zoom; if (camZ < 2.0) camZ = 2.0; }
    if (key == 'z') camZ += zoom;

    if (key == 'k') camPitch += rotStep;
    if (key == 'K') camPitch -= rotStep;

    if (key == 'j') camYaw += rotStep;
    if (key == 'J') camYaw -= rotStep;

    if (key == 'r' || key == 'R') {
        restartGame();
    }

    if (key == 'n' || key == 'N') {
        if (gBoard && gBoard->mapsCount > 0) {
            gBoard->currentMap = (gBoard->currentMap + 1) % gBoard->mapsCount;
            restartGame();  
        }
    }
     
    if (key == 'w' || key == 'W' ||
        key == 'a' || key == 'A' ||
        key == 's' || key == 'S' ||
        key == 'd' || key == 'D')
    {
        characterMove((int)key);
    }

    if (key == 27) exit(0);

    glutPostRedisplay();
}
 
void boardSpecialKey(int key, int x, int y)
{
    (void)x;
    (void)y;

    characterMove(key);   

    glutPostRedisplay();
}

void boardReshape(int w, int h)
{
    if (h == 0) h = 1;

    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 0.1, 200.0);

    glMatrixMode(GL_MODELVIEW);
}

 
void restartGame(void)
{
    if (!gBoard) return;
     
    ClearGhosts();
     
    Map m = getCurrentMap();
    if (m) {
        resetMapVisited(m);
    }
     
    if (gBoard) {
        gBoard->gameStatus = 0;
    }
     
    characterInit();

    glutPostRedisplay();
}

void setEndGame(void)
{
    if (!gBoard) return;

    if (isPacmanAlive() == 0) {
        gBoard->gameStatus = 1;   
        restartGame();  
    }
}