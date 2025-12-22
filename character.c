#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>

#define RAD  0.5;
#define SLICE 32;
#define STACK 24;

void createCharacter() {
  glColour3f(255,255,0);
  glutSolidSphere(RAD , SLICE ,STACK );
}
