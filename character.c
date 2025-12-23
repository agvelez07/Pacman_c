#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>
 
typedef struct{
int r;
int c;
}HousePos;

typedef struct{
int r;
int c;
}Pacman;

int getMapInd(char** m, HousePos* housePosArr){
 int count = 0;
 if(housePosArr && m){
    for(int i = 0; i < xTabSize(); i++){
      for(int j = 0; j < yTabSize(); j++){
        if(m[i][j] == 1){
            housePosArr[count].r = j;
            housePosArr[count].c = i;
            count++;
          }
        }
      }
    }
  }
 return count;
}

HousePos* createHousePos(int* outMax)
{
    int max = xTabSize() * yTabSize();
    if (outMax) *outMax = max;
    return malloc(sizeof(HousePos) * max);
}

void polygon(int a, int b, int c, int d)
{
	 glBegin(GL_POLYGON); 
		glNormal3fv(normals[a]);
		glVertex3fv(vertices[a]); 
		glNormal3fv(normals[b]);
		glVertex3fv(vertices[b]); 
		glNormal3fv(normals[c]);
		glVertex3fv(vertices[c]); 
		glNormal3fv(normals[d]);
		glVertex3fv(vertices[d]);
	glEnd();
}

void  createPacman(void)
{
	glColor3f(1.0f, 1.0f, 0.0f);
	polygon(0, 3, 2, 1); 
	polygon(2, 3, 7, 6); 
 polygon(0, 4, 7, 3); 
 polygon(1, 2, 6, 5); 
	polygon(4, 5, 6, 7); 
	polygon(0, 1, 5, 4);
}

int createCharacter(char** m) {
 int max = 0;
 HousePos* housePosArr = createHousePos(&max);

 getMapInd(m, housePosArr);
 createPacman();
}
