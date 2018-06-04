#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <string.h>

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#include "Game.h"
#include "Aim.h"
#include "Worm.h"
#include "Bluetooth.h"
#include "Shot.h"

// Software SPI (slower updates, more flexible pin options):
//first - Serial clock out (SCLK)
//second - Serial data out (DIN)
//third - Data/Command select (D/C) 
//fourth - LCD chip select (CS)
//fifth - LCD reset (RST) 
Adafruit_PCD8544 display1 = Adafruit_PCD8544(3,2,6,10,5);
Adafruit_PCD8544 display2 = Adafruit_PCD8544(3,2,6,13,9);

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];

//the bluetooth module
extern Adafruit_BluefruitLE_SPI ble;

extern Game game;

extern Worm worm1;
extern Worm worm2;

byte menuPos = 0;

String menu[2] = {
  "Start Game",
  "Quit",
};

void setup()   {
  Serial.begin(9600);

  display1.begin();
  display2.begin();
  display1.setContrast(50);
  display2.setContrast(50);

  display1.clearDisplay();

  Serial.println("Iniciar");
  display1.println("Waiting for bluetooth connection...");
  display1.display();

  delay(2000);
  
  initBluetoothModule();

  display1.clearDisplay();
  display1.println("Bluetooth connected!");
  display1.display();

  display2.clearDisplay();
  display2.println("Bluetooth connected here too!");
  display2.display();
  
  delay(2000);

  showMenu();
}


void loop() {
  if (game.state == 1) {
      detectGround();
  }
  
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;
  
  // Buttons
  if (packetbuffer[1] == 'B') {
    
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';

    //FOR TESTING PURPOSES, BUTTON 3 CHANGES TURNS
    if (pressed && buttnum == BUTTON_3){
      if (game.turn == 1){
        game.turn = 2;
        Serial.print("NEW WORM ID = ");Serial.println(worm2.id);
      } else {
        game.turn = 1;
        Serial.print("NEW WORM ID = ");Serial.println(worm1.id);
      }
      Serial.print("NEW TURN = ");Serial.println(game.turn);
    }

    if (pressed && game.state == 0) {

      if (buttnum == BUTTON_DOWN && menuPos == 0){
          menuPos++;
          showMenu();
          return;
      }

      if(buttnum == BUTTON_UP && menuPos == 1) {
          menuPos--;
          showMenu();
          return;
      }

      if(buttnum == BUTTON_CONFIRM && menuPos == 0) {
        //display1.clearDisplay();
        //display2.clearDisplay();
        gameStart();
        return;
      }

      if(buttnum == BUTTON_CONFIRM && menuPos == 1) {
        display1.clearDisplay();
        display1.println("Exiting the   game...");
        display1.display();
        
        display2.clearDisplay();
        display2.println("Press any key to restart");
        display2.display();
        menuPos=6;
        buttnum=0;
      }

      if(buttnum != 0 && menuPos == 6) {
        menuPos=0;
        showMenu();
      }
    } 
    
    if (pressed && game.state == 1) {
      if (buttnum == BUTTON_RIGHT){
        if (game.turn == 1){
           if (worm1.posX + WORM_WIDTH < DISPLAY_WIDTH){
              worm1.posX++;
              worm1.onGround = false;
           }
        } else {
           if (worm2.posX + WORM_WIDTH < DISPLAY_WIDTH){
              worm2.posX++;
              worm2.onGround = false;
           }
        }
      }

      if (buttnum == BUTTON_LEFT){
        if (game.turn == 1){
           if (worm1.posX > 0){
              worm1.posX--;
              worm1.onGround = false;
           }
        } else {
           if (worm2.posX > 0){
              worm2.posX--;
              worm2.onGround = false;
           }
        }
      }

      detectJump(buttnum);
     
      if (buttnum == BUTTON_CONFIRM){
        game.state = 2;
        aim.step = 6;
        if (game.turn == 1) {
           aim.posX = (worm1.posX + WORM_WIDTH/2) + cos(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.posY = (worm1.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.active1 = true;
           drawDisplay1();
        } else {
           aim.posX = (worm2.posX + WORM_WIDTH/2) - cos(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.posY = (worm2.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.active2 = true;
           drawDisplay2();
        }
       
        return;
      }
    }

    if (pressed && game.state == 2) {
      if (buttnum == BUTTON_UP){
        if (aim.active1 == true && aim.step <= 8){
          aim.step++;
          aim.posY = (worm1.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
          aim.posX = (worm1.posX + WORM_WIDTH/2) + cos(aim.step*(PI/20))*aim.distanceToPlayer;
          drawDisplay1();
        }

        if (aim.active2 == true && aim.step <= 8){
           aim.step++;
           aim.posY = (worm2.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.posX = (worm2.posX + WORM_WIDTH/2) - cos(aim.step*(PI/20))*aim.distanceToPlayer;
           drawDisplay2();
        }
      }

      if (buttnum == BUTTON_DOWN){
        if (aim.active1 == true && aim.step > 0){
           aim.step--;
           aim.posY = (worm1.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.posX = (worm1.posX + WORM_WIDTH/2) + cos(aim.step*(PI/20))*aim.distanceToPlayer;
           drawDisplay1();
        }

        if (aim.active2 == true && aim.step > 0){
           aim.step--;
           aim.posY = (worm2.posY + WORM_HEIGHT/2) - sin(aim.step*(PI/20))*aim.distanceToPlayer;
           aim.posX = (worm2.posX + WORM_WIDTH/2) - cos(aim.step*(PI/20))*aim.distanceToPlayer;
           drawDisplay2();
        }
      }

      if (buttnum == BUTTON_CANCEL){
        game.state = 1;
        aim.active1 = false;
        aim.active2 = false;
        drawDisplay1();
      }

      if (buttnum == BUTTON_CONFIRM){
        shoot();
      }
      
      return;
    }
  }
}

void showMenu() {  
  for (int i = 0; i < (0 + 4); i++) {
    int markerY = (i - 0) * 11;
    
    if (i == menuPos) {
      display1.setTextColor(WHITE, BLACK);
      display1.fillRect(0, markerY, display1.width(), 11, BLACK);
    } else {
      display1.setTextColor(BLACK, WHITE);
      display1.fillRect(0, markerY, display1.width(), 11, WHITE);
    }

    if (i >= 2) {
      continue;
    }
    display1.setCursor(2, markerY + 2);
    display1.print(menu[i]);
  }

  display1.display();
}

void gameStart() {
  game.state = 1;
  drawDisplay1();
  drawDisplay2();
}

void drawWorm(Worm *wormToDraw){
  if (wormToDraw->id == 1){
    if (wormToDraw->currentBitmap == 1){
      display1.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_right1, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 2;
    } else if (wormToDraw->currentBitmap == 2){
      display1.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_right2, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 1;
    }
  } else {
   if (wormToDraw->currentBitmap == 1){
      display2.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_left1, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 2;
    } else if (wormToDraw->currentBitmap == 2){
      display2.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_left2, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 1;
    }
  }
}

void detectJump(int button){
  //jump forward
  if (button == BUTTON_UP){
     if (game.turn == 1) {
        for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           worm1.posX++;
           worm1.posY--;
           drawDisplay1();
        }
  
        for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           worm1.posX++;
           worm1.posY++;
           drawDisplay1();
        }
     } else {
       for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           worm2.posX--;
           worm2.posY--;
           drawDisplay2();
        }
  
        for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           worm2.posX--;
           worm2.posY++;
           drawDisplay2();
        }
     }
     detectGround();
  }

  //jump backward
  if (button == BUTTON_DOWN){
      if (game.turn == 1) {
         for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           worm1.posX--;
           worm1.posY--;
           drawDisplay1();
         }
  
         for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           worm1.posX--;
           worm1.posY++;
           drawDisplay1();
         }
      } else {
        for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           worm2.posX++;
           worm2.posY--;
           drawDisplay2();
         }
  
         for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           worm2.posX++;
           worm2.posY++;
           drawDisplay2();
         }
      }
      detectGround();
  }
}

void detectGround(){

  if (worm1.onGround == false) {
    byte map1_length = sizeof(game.blocks1) / sizeof(Block);
    boolean onGround = false;
    for (byte i=0;i<map1_length;i++){
      if (game.blocks1[i].xIni < worm1.posX + WORM_WIDTH && game.blocks1[i].xIni + game.blocks1[i].width > worm1.posX){
        if (game.blocks1[i].yIni >= worm1.posY && game.blocks1[i].yIni <= worm1.posY + WORM_HEIGHT + 2){
          worm1.onGround = true;
          worm1.posY = game.blocks1[i].yIni - WORM_HEIGHT;
          onGround = true;
          drawDisplay1();
          break;
        }
      }
    }
    if (onGround == false){
      worm1.posY += 2;
      drawDisplay1();
    }

    if (worm1.posY >= DISPLAY_HEIGHT){
      gameOver(worm2);
    }
  }
  
  if (worm2.onGround == false) {
    byte map2_length = sizeof(game.blocks2) / sizeof(Block);
    boolean onGround = false;
    for (byte i=0;i<map2_length;i++){
      if (game.blocks2[i].xIni < worm2.posX + WORM_WIDTH && game.blocks2[i].xIni + game.blocks2[i].width > worm2.posX){
        if (game.blocks2[i].yIni >= worm2.posY && game.blocks2[i].yIni <= worm2.posY + WORM_HEIGHT + 2){
          worm2.onGround = true;
          worm2.posY = game.blocks2[i].yIni - WORM_HEIGHT;
          onGround = true;
          drawDisplay2();
          break;
        }
      }
    }
    if (onGround == false){
      worm2.posY += 2;
      drawDisplay2();
    }

    if (worm2.posY >= DISPLAY_HEIGHT){
      gameOver(worm1);
    }
  }
}

void shoot(){
  shot.posX = aim.posX;
  shot.posY = aim.posY;
  if (game.turn == 2){
    shot.posX += DISPLAY_WIDTH;
  }
  shot.velocity = INITIAL_VELOCITY;
  shot.resistance = HORIZONTAL_RESISTANCE;
  shot.angle = aim.step*(PI/20);
  game.state = 3;

  Serial.println("Shooting");

  boolean hit = false;

  while (!hit){
    delay(75);
     double xSpeed = (INITIAL_VELOCITY/2 * cos(shot.angle)) + shot.resistance;
     double ySpeed = (shot.velocity * sin(shot.angle)) + VERTICAL_RESISTANCE;

     if (xSpeed < 0) {
       xSpeed = 0;
     }

     if (game.turn == 2){
       xSpeed = xSpeed * (-1);
     }

     shot.posX += xSpeed-1;
     shot.posY -= ySpeed;

     Serial.print("Angle = ");Serial.println(shot.angle);
     Serial.print("velocity = ");Serial.println(shot.velocity);
     Serial.print("X Speed = ");Serial.println(xSpeed);
     Serial.print("Y Speed = ");Serial.println(ySpeed);
     Serial.print("Shot X = ");Serial.println(shot.posX);
     Serial.print("Shot Y = ");Serial.println(shot.posY);

     hit = detectShotCollision();

     if (!hit && (shot.posX < 0 || shot.posY > DISPLAY_HEIGHT || shot.posX > DISPLAY_WIDTH*2)){
      game.state = 1;
      if (game.turn == 1){
        game.turn = 2;
      } else {
        game.turn = 1;
      }
      hit = true;
      aim.active1 = false;
      aim.active2 = false;
      drawDisplay1();
     }

     

     shot.velocity = ySpeed;
     shot.resistance -= 0.1;
     if (shot.posX < DISPLAY_WIDTH){
       drawDisplay1();
     } else {
       drawDisplay2();
     }
  }
}

boolean detectShotCollision(){
  if (shot.posX < DISPLAY_WIDTH){
    return false;
  } else {
    byte map2_length = sizeof(game.blocks2) / sizeof(Block);

    for (byte i=0;i<map2_length;i++){
      if (game.blocks2[i].xIni < shot.posX-2 && game.blocks2[i].xIni + game.blocks2[i].width > shot.posX+2){
        if (game.blocks2[i].yIni >= shot.posY-2 && game.blocks2[i].yIni + game.blocks2[i].height <= shot.posY+2){
           Serial.println("BLOCK 2 HIT!!!");
          game.blocks2[i].xIni = 0;
          game.blocks2[i].yIni = 0;
          game.blocks2[i].width = 0;
          game.blocks2[i].height = 0;

          game.state = 1;
          if (game.turn == 1){
            game.turn = 2;
          } else {
            game.turn = 1;
          }
          aim.active1 = false;
          aim.active2 = false;
          //drawDisplay2();
          return true;
        }
      }
    }

    if (worm1.posX < shot.posX-2 && worm1.posX + WORM_WIDTH > shot.posX+2){
        if (worm1.posY >= shot.posY-2 && worm1.posY + WORM_HEIGHT <= shot.posY+2){
           Serial.println("WORM 1 WAS HIT!!!");
           worm1.currentHp--;
           if (worm1.currentHp <= 0){
             gameOver(worm2);
             return true;
           }
        }
    }

    return false;
  }
  
}

void gameOver(Worm worm){
  display1.println("Game Over!");
  display1.print("Player ");display1.print(worm.id);display1.println(" won the game!");
  display1.display();
  game.state = 4;
}

void drawDisplay1(){
 
  byte map1_length = sizeof(game.blocks1) / sizeof(Block);
  display1.clearDisplay();
  for (byte i=0;i<map1_length;i++){
    display1.fillRect(game.blocks1[i].xIni, game.blocks1[i].yIni, game.blocks1[i].width, game.blocks1[i].height, BLACK);
  }
  drawWorm(&worm1);

  if (aim.active1 == true){
    display1.drawCircle(aim.posX, aim.posY, aim.radius, BLACK);
  }

  if (game.state == 3){
    if (shot.posX <= DISPLAY_WIDTH){
      display1.fillCircle(shot.posX, shot.posY, 2, BLACK);
    }
  }
  
  display1.display();
  drawDisplay2();
}

void drawDisplay2(){
  byte map2_length = sizeof(game.blocks2) / sizeof(Block);
  display2.clearDisplay();
  for (byte i=0;i<map2_length;i++){
    display2.fillRect(game.blocks2[i].xIni, game.blocks2[i].yIni, game.blocks2[i].width, game.blocks2[i].height, BLACK);
  }
  drawWorm(&worm2);

  if (aim.active2 == true){
    display2.drawCircle(aim.posX, aim.posY, aim.radius, BLACK);
  }

  if (game.state == 3){
    if (shot.posX > DISPLAY_WIDTH){
      display2.fillCircle(shot.posX - DISPLAY_WIDTH, shot.posY, 2, BLACK);
    }
  }
  
  display2.display();
}

void debugGame(){
  Serial.println("-------GAME--------");
  Serial.print("Turn = ");Serial.println(game.turn);
  Serial.print("State = ");Serial.println(game.state);
  Serial.print("Blocks1 has size of ");Serial.print(sizeof(game.blocks1));Serial.print(" and lenght of ");Serial.println(sizeof(game.blocks1) / sizeof(Block));
  Serial.print("Blocks2 has size of ");Serial.print(sizeof(game.blocks2));Serial.print(" and lenght of ");Serial.println(sizeof(game.blocks2) / sizeof(Block));
  Serial.println("-------------------");
}

void debugPins(){
  digitalWrite(2,HIGH);
  digitalWrite(6,HIGH);
  digitalWrite(13,HIGH);
  Serial.print("Pin 2 = ");Serial.println(digitalRead(2));
  Serial.print("Pin 3 = ");Serial.println(digitalRead(3));
  Serial.print("Pin 5 = ");Serial.println(digitalRead(5));
  Serial.print("Pin 6 = ");Serial.println(digitalRead(6));
  Serial.print("Pin 10 = ");Serial.println(digitalRead(10));
  Serial.print("Pin 12 = ");Serial.println(digitalRead(12));
  Serial.print("Pin 13 = ");Serial.println(digitalRead(13));
}

