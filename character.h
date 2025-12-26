#ifndef CHARACTERH
#define CHARACTERH

typedef struct pacman* Pacman;

void characterInit(char** map);
void characterDraw(void);
int characterMove(unsigned char key);
Pacman characterGet(void);

#endif
