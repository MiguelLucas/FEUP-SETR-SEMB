#ifndef WORM_H
#define WORM_H

#include "Utilities.h"

#define WORM_WIDTH 14
#define WORM_HEIGHT 11

struct Worm {
  byte id;
  byte posX;
  byte posY;
  byte currentHp;

  //jumping settings
  byte jumpDelay;
  byte jumpHeight;

  //bitmap settings
  byte currentBitmap;

  //physics settings;
  boolean onGround;
};

unsigned char* getBitmap(Worm worm);

#endif
