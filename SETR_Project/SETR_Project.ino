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

// Software SPI (slower updates, more flexible pin options):
//first - Serial clock out (SCLK)
//second - Serial data out (DIN)
//third - Data/Command select (D/C) 
//fourth - LCD chip select (CS)
//fifth - LCD reset (RST) 
Adafruit_PCD8544 display1 = Adafruit_PCD8544(3,2,6,10,5);
Adafruit_PCD8544 display2 = Adafruit_PCD8544(3,2,6,13,12);

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
  display2.clearDisplay();

  debugGame();
  
  
  Serial.println("Iniciar");
  display1.println("Waiting for bluetooth connection...");

  display1.display();
  //display2.display();

  delay(2000);
  
  initBluetoothModule();

  display1.clearDisplay();
  //display2.clearDisplay();
  display1.println("Bluetooth connected!");
  display1.display();
  //display2.display();
  
  delay(2000);
  
  showMenu();

  //drawInitialTerrain();
  //gameStart();
}


void loop() {
  if (game.state == 1) {
      Serial.print("ONGROUND = ");Serial.println(worm1.onGround);
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
              clearWorm(worm1);
              worm1.posX++;
              drawWorm(&worm1);
              worm1.onGround = false;
           }
        } else {
           if (worm2.posX + WORM_WIDTH < DISPLAY_WIDTH){
              clearWorm(worm2);
              worm2.posX++;
              drawWorm(&worm2);
              worm2.onGround = false;
           }
        }
      }

      if (buttnum == BUTTON_LEFT){
        if (game.turn == 1){
           if (worm1.posX > 0){
              clearWorm(worm1);
              worm1.posX--;
              drawWorm(&worm1);
              worm1.onGround = false;
           }
        } else {
           if (worm2.posX > 0){
              clearWorm(worm2);
              worm2.posX--;
              drawWorm(&worm2);
              worm2.onGround = false;
           }
        }
      }

      detectJump(buttnum);
     
      
      if (buttnum == BUTTON_CONFIRM){
        game.state = 2;
        if (game.turn == 1) {
           aim.posX = worm1.posX + aim.distanceToPlayer;
           aim.posY = worm1.posY - aim.distanceToPlayer;
        } else {
           aim.posX = worm2.posX - aim.distanceToPlayer;
           aim.posY = worm2.posY - aim.distanceToPlayer;
        }
       
        drawAimingX(BLACK);
        return;
      }
    }

    if (pressed && game.state == 2) {
      if (buttnum == BUTTON_UP && aim.posX >= worm1.posX){
        drawAimingX(WHITE);
        aim.posY--;
        aim.posX--;
        drawAimingX(BLACK);
      }

      if (buttnum == BUTTON_DOWN && aim.posY <= worm1.posY + WORM_HEIGHT){
        drawAimingX(WHITE);
        aim.posY++;
        aim.posX++;
        drawAimingX(BLACK);
      }

      if (buttnum == BUTTON_CANCEL){
        game.state = 1;
        drawAimingX(WHITE);
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
  display1.clearDisplay();
  display2.clearDisplay();
  
  //display1.println("Loading map...");
  //display2.println("Loading map...");

  display1.display();
  display2.display();

   drawWorm(&worm1);

   game.state = 1;

  drawInitialMap();
  display1.display();
  //display2.display();
}

void drawInitialMap(){
  
  display1.clearDisplay();
  display2.clearDisplay();

  display1.println("Loading map...");
  display1.display();
  display2.display();

  delay(1000);

  display1.clearDisplay();
  display2.clearDisplay();
  
  byte map1_length = sizeof(game.blocks1) / sizeof(Block);
  byte map2_length = sizeof(game.blocks2) / sizeof(Block);
  Serial.print("Pin 10 = ");Serial.println(digitalRead(10));

  Serial.print("Pin 10 = ");Serial.println(digitalRead(10));
  Serial.print("Pin 13 = ");Serial.println(digitalRead(13));
  Serial.print("Pin 12 = ");Serial.println(digitalRead(12));
  for (byte i=0;i<map1_length;i++){
    display1.fillRect(game.blocks1[i].xIni, game.blocks1[i].yIni, game.blocks1[i].width, game.blocks1[i].height, BLACK);
  }

  drawWorm(&worm1);
  display1.display();
   //digitalWrite(5,LOW);

  for (byte i=0;i<map2_length;i++){
    display2.fillRect(game.blocks2[i].xIni, game.blocks2[i].yIni, game.blocks2[i].width, game.blocks2[i].height, BLACK);
  }

  display2.display();
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
    //display2.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp1, WORM_WIDTH, WORM_HEIGHT, WHITE);
    Serial.println("Displaying WORM 1:");
    display1.display();
  } else {
   if (wormToDraw->currentBitmap == 1){
      display2.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_left1, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 2;
    } else if (wormToDraw->currentBitmap == 2){
      display2.drawBitmap(wormToDraw->posX, wormToDraw->posY, worm_bmp_left2, WORM_WIDTH, WORM_HEIGHT, BLACK);
      wormToDraw->currentBitmap = 1;
    }
    Serial.println("Displaying WORM 2: ");
    display2.display();
  }
}

void clearWorm(Worm wormToDraw){
   if (wormToDraw.id == 1){
      if (wormToDraw.currentBitmap == 1){
        display1.drawBitmap(wormToDraw.posX, wormToDraw.posY, worm_bmp_right2, WORM_WIDTH, WORM_HEIGHT, WHITE);
    } else if (wormToDraw.currentBitmap == 2){
        display1.drawBitmap(wormToDraw.posX, wormToDraw.posY, worm_bmp_right1, WORM_WIDTH, WORM_HEIGHT, WHITE);
    }
     display1.display();
   } else {
     if (wormToDraw.currentBitmap == 1){
      display2.drawBitmap(wormToDraw.posX, wormToDraw.posY, worm_bmp_left2, WORM_WIDTH, WORM_HEIGHT, WHITE);
    } else if (wormToDraw.currentBitmap == 2){
      display2.drawBitmap(wormToDraw.posX, wormToDraw.posY, worm_bmp_left1, WORM_WIDTH, WORM_HEIGHT, WHITE);
    }
    display2.display();
   }
}

void drawAimingX(int color){
  if (game.turn == 1){
    display1.drawCircle(aim.posX, aim.posY, aim.radius, color);
    display1.display();
  } else {
    display2.drawCircle(aim.posX, aim.posY, aim.radius, color);
    display2.display();
  }
}

void detectJump(int button){
  //jump forward
  if (button == BUTTON_UP){
     if (game.turn == 1) {
        for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           clearWorm(worm1);
           worm1.posX++;
           worm1.posY--;
           drawWorm(&worm1);
        }
  
        for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           clearWorm(worm1);
           worm1.posX++;
           worm1.posY++;
           drawWorm(&worm1);
        }
     } else {
       for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           clearWorm(worm2);
           worm2.posX--;
           worm2.posY--;
           drawWorm(&worm2);
        }
  
        for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           clearWorm(worm2);
           worm2.posX--;
           worm2.posY++;
           drawWorm(&worm2);
        }
     }
     detectGround();
  }

  //jump backward
  if (button == BUTTON_DOWN){
      if (game.turn == 1) {
         for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           clearWorm(worm1);
           worm1.posX--;
           worm1.posY--;
           drawWorm(&worm1);
         }
  
         for (int i=1;i<worm1.jumpHeight;i++){
           delay(worm1.jumpDelay);
           clearWorm(worm1);
           worm1.posX--;
           worm1.posY++;
           drawWorm(&worm1);
         }
      } else {
        for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           clearWorm(worm2);
           worm2.posX++;
           worm2.posY--;
           drawWorm(&worm2);
         }
  
         for (int i=1;i<worm2.jumpHeight;i++){
           delay(worm2.jumpDelay);
           clearWorm(worm2);
           worm2.posX++;
           worm2.posY++;
           drawWorm(&worm2);
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
      Serial.print("I = ");Serial.println(i);
      Serial.print("Block1 X = ");Serial.println(game.blocks1[i].xIni);
      Serial.print("Block1 Width = ");Serial.println(game.blocks1[i].width);
      Serial.print("Worm1 X = ");Serial.println(worm1.posX);
      Serial.print("Worm1 pos = ");Serial.println(worm1.posX + WORM_WIDTH + 1);
      if (game.blocks1[i].xIni < worm1.posX + WORM_WIDTH && game.blocks1[i].xIni + game.blocks1[i].width > worm1.posX){
        if (game.blocks1[i].yIni >= worm1.posY && game.blocks1[i].yIni <= worm1.posY + WORM_HEIGHT + 2){
          Serial.println("--------GROUND 1--------");
          clearWorm(worm1);
          worm1.onGround = true;
          worm1.posY = game.blocks1[i].yIni - WORM_HEIGHT;
          onGround = true;
          drawWorm(&worm1);
          break;
        }
      }
    }
    if (onGround == false){
      clearWorm(worm1);
      worm1.posY += 2;
      drawWorm(&worm1);
      //if (worm1.posY = DISPLAY_HEIGHT
    }
  }
  
  /*if (worm2.onGround == false) {
    byte map2_length = sizeof(game.blocks2) / sizeof(Block);
    boolean onGround = false;
    for (byte i=0;i<map2_length;i++){
      Serial.print("I = ");Serial.println(i);
      Serial.print("Block2 Y = ");Serial.println(game.blocks2[i].yIni);
      Serial.print("Worm2 Y = ");Serial.println(worm2.posY);
      Serial.print("Worm2 pos = ");Serial.println(worm2.posY + WORM_HEIGHT + 1);
      if (game.blocks2[i].xIni >= worm2.posX + WORM_WIDTH && game.blocks2[i].xIni + game.blocks2[i].width <= worm2.posX){
        if (game.blocks2[i].yIni >= worm2.posY && game.blocks2[i].yIni <= worm2.posY + WORM_HEIGHT + 2){
          Serial.println("--------GROUND 2--------");
          worm2.onGround = true;
          worm2.posY = game.blocks2[i].yIni - WORM_HEIGHT;
          onGround = true;
          break;
        }
      }
    }
    if (onGround == false){
      clearWorm(worm2);
      worm2.posY += 2;
      drawWorm(&worm2);
    }
  }*/
}

void debugGame(){
  Serial.println("-------GAME--------");
  Serial.print("Turn = ");Serial.println(game.turn);
  Serial.print("State = ");Serial.println(game.state);
  Serial.print("Blocks1 has size of ");Serial.print(sizeof(game.blocks1));Serial.print(" and lenght of ");Serial.println(sizeof(game.blocks1) / sizeof(Block));
  Serial.print("Blocks2 has size of ");Serial.print(sizeof(game.blocks2));Serial.print(" and lenght of ");Serial.println(sizeof(game.blocks2) / sizeof(Block));
  Serial.println("-------------------");
}

