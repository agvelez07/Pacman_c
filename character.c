#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>

#define RAD  0.5;
#define SLICE 32;
#define STACK 24;

struct housePos{
int r;
int c;
}HousePos;

int getMapInd(char** m, int index[][2]){
  int r = 0;
  for(int i = 0; i < xTable(); i++){
    for(int j = 0; j < yTable(); j++){
      if(m[i][j] == 1){
        index[r][0] = m[i];
        index[r][1] = m[j];
        r++;
      }
    }
  }
  return index;
}

HousePos createhousePos(){
  int max = int max = xTabSize() * yTabSize();
  HousePos* housePost = malloc(SizeOf(HousePos * max);
  return housePos;  
}

int createCharacter(char** m) {
  createhousePos();
  getMapInd(m, index);

}
