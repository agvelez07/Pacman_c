#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>
 
type struct{
int r;
int c;
}HousePos;

void getMapInd(char** m){
  if(housePos){
  {
    int count = 0;
    for(int i = 0; i < xTable(); i++){
      for(int j = 0; j < yTable(); j++){
        if(m[i][j] == 1){
            housePost[count].r = i;
            housePost[count].c = j;
            count++;
          }
        }
      }
    }
  }
}

HousePos createhousePos(){
  int max = int max = xTabSize() * yTabSize();
  HousePos* housePost = malloc(SizeOf(HousePos * max);
  return housePos;  
}

int createCharacter(char** m) {
  createhousePos();
  getMapInd(m);

}
