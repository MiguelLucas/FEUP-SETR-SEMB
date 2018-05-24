#ifndef GAME_H
#define GAME_H

#include <Arduino.h> //needed for Serial.println
#include <string.h>

#define WIDTH 84
#define HEIGHT 48

const char TERRAIN_ICON = 'T';

char game_map[(WIDTH*2)/4][HEIGHT/4];
void init_game_map(){
  
}


struct Game {
  char game_map[WIDTH*2][HEIGHT*2];
  String player;
};

#endif
