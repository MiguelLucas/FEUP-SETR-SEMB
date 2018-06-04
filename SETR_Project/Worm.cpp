#include "Worm.h"

Worm worm1 = {1, 10, 10, 5, 5, 50, 8, 1, false};
Worm worm2 = {2, 30, 25, 5, 5, 50, 8, 1, false};

unsigned char* getBitmap(Worm worm){
  if (worm.id == 1) {
    if (worm.currentBitmap == 1){
      worm.currentBitmap = 2;
      return worm_bmp_right1;
    } else if (worm.currentBitmap == 2) {
      worm.currentBitmap = 1;
      return worm_bmp_right2;
    }
  } else {
    if (worm.currentBitmap == 1){
      worm.currentBitmap = 2;
      return worm_bmp_left1;
    } else if (worm.currentBitmap == 2) {
      worm.currentBitmap = 1;
      return worm_bmp_left2;
    }
  }
}

