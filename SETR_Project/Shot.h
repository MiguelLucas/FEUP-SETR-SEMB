#ifndef SHOT_H
#define SHOT_H

#define INITIAL_VELOCITY 8
#define INITIAL_VERTICAL_RESISTANCE 0.2
#define INITIAL_HORIZONTAL_RESISTANCE 0.1

struct Shot {
  byte posX;
  byte posY;
  double velocity;
  double angle;
  double v_resistance;
  double h_resistance;
};


Shot shot = {0, 0, 20, 0, 0.4, 0.2};

#endif
