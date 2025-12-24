#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

typedef struct {
    int c, r;
    TileStatus* tileStatus;   
    int wallMode;        // 0/1
    int gameOver;        // 0/1
} Board;


typedef struct {
    int wallOn;    // 1 = buraco/limite (map==0)
    int visited;   // 1 = pacman passou
    int occupied;  // 0 vazio | 1 pacman | 2 ghost
} TileStatus;
