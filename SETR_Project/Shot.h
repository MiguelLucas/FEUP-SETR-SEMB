#ifndef SHOT_H
#define SHOT_H

#define INITIAL_VELOCITY 20
#define VERTICAL_RESISTANCE -1.5
#define HORIZONTAL_RESISTANCE -1

struct Shot {
  byte posX;
  byte posY;
  double velocity;
  double angle;
  double resistance;
};


Shot shot = {0, 0, 20, 0, 8};

#endif
