#ifndef AIM_H
#define AIM_H

struct Aim {
  byte posX;
  byte posY;
  byte distanceToPlayer;
  byte radius;
  byte step;
  boolean active1;
  boolean active2;
};

Aim aim = {10, 10, 15, 3, 10, false, false};

#endif
