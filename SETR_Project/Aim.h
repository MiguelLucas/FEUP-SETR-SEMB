#ifndef AIM_H
#define AIM_H

struct Aim {
  byte posX;
  byte posY;
  byte distanceToPlayer;
  byte radius;
};

Aim aim = {10, 10, 10, 4};

#endif
