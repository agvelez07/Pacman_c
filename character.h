#ifndef CHARACTER_H
#define CHARACTER_H

typedef struct pacman* Pacman;

void characterInit(void);
void characterDraw(void);
int characterMove(unsigned char key);

int getPacmanRow(Pacman p);
int getPacmanCol(Pacman p);

#endif
