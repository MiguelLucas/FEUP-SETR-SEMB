#ifndef GAME_H
#define GAME_H

#include <Arduino.h> //needed for Serial.println
#include <string.h>

#define DISPLAY_WIDTH 84
#define DISPLAY_HEIGHT 48

struct Block {
  byte xIni;
  byte yIni;
  byte width;
  byte height;
};
/**
 * States: 0 - menu; 1 - game; 2- aiming
 * Turn: 1 - Player 1; 2 - Player 2
 */
struct Game {
  byte state;
  byte turn;
  Block blocks1[2];
  Block blocks2[3];
};

#endif
