#ifndef CHARACTER_H
#define CHARACTER_H

typedef struct pacman* Pacman;
typedef struct ghost* Ghost;

void ghostsDraw(Ghost* ghost);

void characterInit(void);
void characterDraw(void);
int characterMove(unsigned char key);

void updateMouth(void);
void timer(int v);

int getPacmanRow(Pacman p);
int getPacmanCol(Pacman p);


int isPacmanAlive(void);

int checkWinCondition(void);
void updateAllGhosts(void);

#endif
