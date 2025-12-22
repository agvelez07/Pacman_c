#include "pm-maps.h"
#include <stdlib.h>
#include <stdio.h>

#define rad  0;
#define slice 32;
#define stacl 24;

void createCharacter(int x) {
  glutSolidSphere(	rad , slices , stacks );
  glColour3f(255,255,0);
}
