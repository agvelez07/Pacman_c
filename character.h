#pragma once

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct {
    int r; 
    int c; 
} Pacman;
 
void character_init(char** map);
 
void character_draw(void);
 
Pacman character_get_pos(void);
 
int character_try_move(char** map, int dc, int dr);
