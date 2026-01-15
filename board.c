#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

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
    int paused;       
    Pacman pacman;
    Ghost* ghosts;
};

static GLfloat camX = 0.0;
static GLfloat camY = 0.0;
static GLfloat camZ = 2.0;

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

int getBoardWallMode(void)
{
    return gBoard ? gBoard->wallMode : 0;
}

int setBoardWallMode(void)
{
    if (!gBoard) return 0;
    gBoard->wallMode = !gBoard->wallMode;

    if (gBoard->wallMode == 0) {
        Map m = getCurrentMap();
        if (m) {
            resetWalls(m);
        }
    }

    return gBoard->wallMode;
}

 
int getBoardPaused(void)
{
    return gBoard ? gBoard->paused : 0;
}

void toggleBoardPaused(void)
{
    if (!gBoard) return;
    gBoard->paused = !gBoard->paused;
}
 
int getBoardGameStatus(void)
{
    return gBoard ? gBoard->gameStatus : 0;
}

void setBoardGameStatus(int status)
{
    if (!gBoard) return;
    gBoard->gameStatus = status;
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

void ClearGhosts(void)
{
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

            if (!cellIsHouse(cell)) continue;

            int rr = (rows - 1) - r;

            glPushMatrix();
            glTranslatef((GLfloat)(c * 2), (GLfloat)(rr * 2), 0.0);
            glScalef(0.9, 0.9, 0.2);
            colorCube(cell);
            glPopMatrix();

            if (hasWall(cell)) {
                glPushMatrix();
                glTranslatef((GLfloat)(c * 2), (GLfloat)(rr * 2), 0.4);
                glScalef(0.9, 0.9, 0.2);
                drawWall();
                glPopMatrix();

                Cell right = cellAt(m, r, c + 1);
                if (right && cellIsHouse(right) && hasWall(right)) {
                    glPushMatrix();
                    glTranslatef((GLfloat)(c * 2 + 0.9), (GLfloat)(rr * 2), 0.4);
                    glScalef(0.2, 0.36, 0.4);
                    drawWallConnection();
                    glPopMatrix();
                }

                Cell down = cellAt(m, r + 1, c);
                int rrDown = (rows - 1) - (r + 1);
                if (down && cellIsHouse(down) && hasWall(down)) {
                    glPushMatrix();
                    glTranslatef((GLfloat)(c * 2), (GLfloat)(rrDown * 2 + 0.9), 0.4);
                    glScalef(0.36, 0.2, 0.4);
                    drawWallConnection();
                    glPopMatrix();
                }
            }
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
    gBoard->paused = 0;   
    gBoard->pacman = NULL;
    gBoard->ghostCount = 0;
    gBoard->ghosts = NULL;

    gBoard->mapsCount = readAllMaps(&gBoard->maps, mapFile);
    if (gBoard->mapsCount <= 0) exit(1);

    gBoard->currentMap = 0;

    characterInit();
}
 
static void drawTextBackground(void)
{
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glColor4f(0.0, 0.0, 0.0, 0.8);

    glBegin(GL_QUADS);
    glVertex2f(W / 2 - 200, H / 2 - 40);
    glVertex2f(W / 2 + 200, H / 2 - 40);
    glVertex2f(W / 2 + 200, H / 2 + 40);
    glVertex2f(W / 2 - 200, H / 2 + 40);
    glEnd();

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

static void drawText(const char* text)
{
    glDisable(GL_DEPTH_TEST);
    glColor3f(1.0, 1.0, 1.0);

    glRasterPos2f(W / 2 - 100, H / 2);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }

    glEnable(GL_DEPTH_TEST);
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

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, W, 0.0, H);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    Pacman p = getPacman();

    if (p) {
        char stepText[50];
        sprintf(stepText, "Steps: %d", getPacmanSteps(p));
        glColor3f(1.0, 1.0, 1.0);
        glRasterPos2f(10, H - 25);
        for (const char* c = stepText; *c != '\0'; c++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
        }
    }

    if (gBoard && gBoard->paused) {
        drawTextBackground();
        drawText("Game Paused - Press P to continue");
    }

    if (gBoard && gBoard->gameStatus == 1) {
        drawTextBackground();
        drawText("GAME OVER!  - Press R to Reset");
    }

    else if (gBoard && gBoard->gameStatus == 2) {
        drawTextBackground();
        drawText("Winner!  - Press R to reset");
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

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
     
    if (key == 'p' || key == 'P') {
        toggleBoardPaused();
    }

    if (key == 'w' || key == 'W') {
        setBoardWallMode();
    }

    if (key == 'n' || key == 'N') {
        if (gBoard && gBoard->mapsCount > 0) {
            gBoard->currentMap = (gBoard->currentMap + 1) % gBoard->mapsCount;
            restartGame();
        }
    }

    if (key == 27) exit(0);

    glutPostRedisplay();
}

void boardSpecialKey(int key, int x, int y)
{
    (void)x;
    (void)y;
 
    if (gBoard && (gBoard->paused || gBoard->gameStatus != 0)) {
        return;
    }

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
        resetWalls(m);
    }

    if (gBoard) {
        gBoard->gameStatus = 0;
        gBoard->wallMode = 0;
        gBoard->paused = 0;  
    }

    characterInit();

    glutPostRedisplay();
}

void setEndGame(void)
{
    if (!gBoard) return;

    if (isPacmanAlive() == 0) {
        gBoard->gameStatus = 1;   
    }
}