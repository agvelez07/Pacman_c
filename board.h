#ifndef BOARD_H
#define BOARD_H

#include "pm-maps.h"
#include "character.h"

typedef struct board* Board;

void boardInit(const char* mapFile);
void boardDestroy(void);

Board getBoard(void);

Map getCurrentMap(void);
int getBoardMapCount(void);
int getCurrentMapIndex(void);


Pacman getPacman(void);
int setBoardPacman(Pacman p);
int setBoardGhosts(Ghost* ghosts, int ghostCount);


void boardDisplay(void);
void boardReshape(int w, int h);
void boardKey(unsigned char key, int x, int y);

#endif
