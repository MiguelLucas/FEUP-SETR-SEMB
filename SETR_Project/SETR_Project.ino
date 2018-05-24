#include <Adafruit_ATParser.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BLEBattery.h>
#include <Adafruit_BLEEddystone.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLEMIDI.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>

#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Adafruit_BLE.h"

#include <string.h>

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

#include "game.h"

// Software SPI (slower updates, more flexible pin options):
//first - Serial clock out (SCLK) - 7
//second - Serial data out (DIN) - 6
//third - Data/Command select (D/C) - 5
//fourth - LCD chip select (CS) - 3
//fifth - LCD reset (RST) - 4
//Adafruit_PCD8544 display1 = Adafruit_PCD8544(3, 0, 5, 2, 6);
//Adafruit_PCD8544 display1 = Adafruit_PCD8544(9, 10, 11, 13, 12);
Adafruit_PCD8544 display1 = Adafruit_PCD8544(3,2,1);
//Game game;

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
    #define MODE_LED_BEHAVIOUR          "MODE"

//CS = 8
//IRQ = 7
//RST = 4
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// function prototypes over in packetparser.cpp
uint8_t readPacket(Adafruit_BLE *ble, uint16_t timeout);
float parsefloat(uint8_t *buffer);
void printHex(const uint8_t * data, const uint32_t numBytes);

// the packet buffer
extern uint8_t packetbuffer[];


#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16
#define LOGO16_GLCD_WIDTH  16

static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

void setup()   {
  Serial.begin(9600);
  //game_map[1][1] = 'R';
  
//  game.player = "cenas";
  //init_game_map();
  // init done

  


  display1.begin();
  //display2.begin();
  display1.setContrast(50);
  //display2.setContrast(50);

  display1.clearDisplay();
  //display2.clearDisplay();
  
  Serial.println("Iniciar");
  display1.println("Loading...");
  

  display1.display();
  //display2.display();

  delay(2000);
  //display1.clearDisplay();
  //display1.println("Waiting for bluetooth connection...");
  //display1.display();
  
  //initBluetoothModule();
  
  delay(2000);
  Serial.println("Bluetooth connected");
  display1.begin();
  display1.clearDisplay();
  display1.println("Bluetooth connected!");
  display1.display();
  delay(2000);
  // you can change the contrast around to adapt the display
  // for the best viewing!
  


  //drawInitialTerrain();

  /*for (int i=display2.width()/2; i <= display2.width()*2 + 10; i+=4) {
    //if (i == '\n') continue;
    if (i <= display1.width()){
        display1.fillCircle(i-4, display1.height()/2, 7, WHITE);
        display1.fillCircle(i, display1.height()/2, 7, BLACK);
        display1.display();
    } 

    if (i >= display1.width()+10){
        display2.fillCircle((i-4) - display1.width(), display2.height()/2, 7, WHITE);
        display2.fillCircle(i - display1.width(), display2.height()/2, 7, BLACK);
        //display1.fillCircle(i - display1.width(), display2.height()/2, 7, WHITE);
        //display1.fillCircle(i-4 - display1.width(), display2.height()/2, 7, WHITE);
        display2.display();
    }

    if (i > display1.width()-10){
      display2.fillCircle(i - display1.width(), display2.height()/2, 7, WHITE);
      display2.fillCircle(i-4 - display1.width(), display2.height()/2, 7, WHITE);
    }

    display1.display();
    if (i >= display1.width()-10){
        display2.fillCircle((i-4) - display1.width(), display2.height()/2, 7, WHITE);
        display2.fillCircle(i - display1.width(), display2.height()/2, 7, BLACK);
        //display1.fillCircle(i - display1.width(), display2.height()/2, 7, WHITE);
        //display1.fillCircle(i-4 - display1.width(), display2.height()/2, 7, WHITE);
        display2.display();
    }
    delay(200);
  }
   /* delay(200);
    //display1.clearDisplay();
    //display2.clearDisplay();
  }*/
  /*display1.setTextColor(WHITE, BLACK); // 'inverted' text
  display1.println(3.141592);
  display1.setTextSize(2);
  display1.setTextColor(BLACK);
  display1.print("0x"); display1.println(0xDEADBEEF, HEX);
  display1.display();
  delay(2000);
  /*display2.fillCircle(-10, display2.height()/2, 10, BLACK);
  display2.display();
  delay(2000);

  // draw a single pixel
  display1.drawPixel(10, 10, BLACK);
  display1.display();
  delay(2000);
  display1.clearDisplay();

  display2.fillCircle(display2.width()/2, display2.height()/2, 10, BLACK);
  display2.display();
  delay(2000);
  display2.clearDisplay();

  // draw many lines
  testdrawline();
  display1.display();
  delay(2000);
  display1.clearDisplay();

  // draw rectangles
  testdrawrect();
  display1.display();
  delay(2000);
  display1.clearDisplay();

  // draw multiple rectangles
  testfillrect();
  display1.display();
  delay(2000);
  display1.clearDisplay();

  // draw mulitple circles
  testdrawcircle();
  display1.display();
  delay(2000);
  display1.clearDisplay();

  // draw a circle, 10 pixel radius
  display1.fillCircle(display1.width()/2, display1.height()/2, 10, BLACK);
  display1.display();
  delay(2000);
  display1.clearDisplay();

  testdrawroundrect();
  delay(2000);
  display1.clearDisplay();

  testfillroundrect();
  delay(2000);
  display1.clearDisplay();

  testdrawtriangle();
  delay(2000);
  display1.clearDisplay();
   
  testfilltriangle();
  delay(2000);
  display1.clearDisplay();

  // draw the first ~12 characters in the font
  testdrawchar();
  display1.display();
  delay(2000);
  display1.clearDisplay();

  // text display tests
  display1.setTextSize(1);
  display1.setTextColor(BLACK);
  display1.setCursor(0,0);
  display1.println("Hello, world!");
  display1.setTextColor(WHITE, BLACK); // 'inverted' text
  display1.println(3.141592);
  display1.setTextSize(2);
  display1.setTextColor(BLACK);
  display1.print("0x"); display1.println(0xDEADBEEF, HEX);
  display1.display();
  delay(2000);

  // rotation example
  display1.clearDisplay();
  display1.setRotation(1);  // rotate 90 degrees counter clockwise, can also use values of 2 and 3 to go further.
  display1.setTextSize(1);
  display1.setTextColor(BLACK);
  display1.setCursor(0,0);
  display1.println("Rotation");
  display1.setTextSize(2);
  display1.println("Example!");
  display1.display();
  delay(2000);

  // revert back to no rotation
  display1.setRotation(0);

  // miniature bitmap display
  display1.clearDisplay();
  display1.drawBitmap(30, 16,  logo16_glcd_bmp, 16, 16, 1);
  display1.display();

  // invert the display
  display1.invertDisplay(true);
  delay(1000); 
  display1.invertDisplay(false);
  delay(1000); 

  // draw a bitmap icon and 'animate' movement
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT);*/
}


void loop() {
  uint8_t len = readPacket(&ble, BLE_READPACKET_TIMEOUT);
  if (len == 0) return;

  // Buttons
  if (packetbuffer[1] == 'B') {
    uint8_t buttnum = packetbuffer[2] - '0';
    boolean pressed = packetbuffer[3] - '0';
    Serial.print ("Button "); Serial.print(buttnum);
    if (pressed) {
      if (buttnum == 4){
        //display1.clearDisplay();
        display1.println("Button 4 clicked!");
        display1.display();
        Serial.println("aqui");
      }
      Serial.println(" pressed");
    } else {
      Serial.println(" released");
    }
  }
}
void initBluetoothModule() {
  Serial.println(F("Adafruit Bluefruit App Controller Example"));
  Serial.println(F("-----------------------------------------"));

  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }


  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in Controller mode"));
  Serial.println(F("Then activate/use the sensors, color picker, game controller, etc!"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));

  // LED Activity command is only supported from 0.6.6
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    // Change Mode LED Activity
    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
  }

  // Set Bluefruit to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

void drawInitialTerrain(){
 
  
  
  //display1.println("outro teste");
   //display1.clearDisplay();
   //display2.clearDisplay();
   display1.println("Loading map...");
   display1.display();
  for (int i=0;i<WIDTH;i++){
    for (int j=HEIGHT-(HEIGHT/4);j<HEIGHT;j++){
      display1.drawPixel(i, j, BLACK);
      //display2.drawPixel(i, j, BLACK);
      //display1.display();
      Serial.print("I = ");
      Serial.print(i);
      Serial.print(" J = ");
      Serial.println(j);
      //if (game_map[j][i] == TERRAIN_ICON)
       // display1.drawPixel(i, j, BLACK);
    }
  }
  display1.display();
  //display2.display();
  
  
}

void testdrawbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[NUMFLAKES][3];
  randomSeed(666);     // whatever seed
 
  // initialize
  for (uint8_t f=0; f< NUMFLAKES; f++) {
    icons[f][XPOS] = random(display1.width());
    icons[f][YPOS] = 0;
    icons[f][DELTAY] = random(5) + 1;
    
    Serial.print("x: ");
    Serial.print(icons[f][XPOS], DEC);
    Serial.print(" y: ");
    Serial.print(icons[f][YPOS], DEC);
    Serial.print(" dy: ");
    Serial.println(icons[f][DELTAY], DEC);
  }

  while (1) {
    // draw each icon
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display1.drawBitmap(icons[f][XPOS], icons[f][YPOS], logo16_glcd_bmp, w, h, BLACK);
    }
    display1.display();
    delay(200);
    
    // then erase it + move it
    for (uint8_t f=0; f< NUMFLAKES; f++) {
      display1.drawBitmap(icons[f][XPOS], icons[f][YPOS],  logo16_glcd_bmp, w, h, WHITE);
      // move it
      icons[f][YPOS] += icons[f][DELTAY];
      // if its gone, reinit
      if (icons[f][YPOS] > display1.height()) {
  icons[f][XPOS] = random(display1.width());
  icons[f][YPOS] = 0;
  icons[f][DELTAY] = random(5) + 1;
      }
    }
   }
}


void testdrawchar(void) {
  display1.setTextSize(1);
  display1.setTextColor(BLACK);
  display1.setCursor(0,0);

  for (uint8_t i=0; i < 168; i++) {
    if (i == '\n') continue;
    display1.write(i);
    //if ((i > 0) && (i % 14 == 0))
      //display.println();
  }    
  display1.display();
}

void testdrawcircle(void) {
  for (int16_t i=0; i<display1.height(); i+=2) {
    display1.drawCircle(display1.width()/2, display1.height()/2, i, BLACK);
    display1.display();
  }
}

void testfillrect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display1.height()/2; i+=3) {
    // alternate colors
    display1.fillRect(i, i, display1.width()-i*2, display1.height()-i*2, color%2);
    display1.display();
    color++;
  }
}

void testdrawtriangle(void) {
  for (int16_t i=0; i<min(display1.width(),display1.height())/2; i+=5) {
    display1.drawTriangle(display1.width()/2, display1.height()/2-i,
                     display1.width()/2-i, display1.height()/2+i,
                     display1.width()/2+i, display1.height()/2+i, BLACK);
    display1.display();
  }
}

void testfilltriangle(void) {
  uint8_t color = BLACK;
  for (int16_t i=min(display1.width(),display1.height())/2; i>0; i-=5) {
    display1.fillTriangle(display1.width()/2, display1.height()/2-i,
                     display1.width()/2-i, display1.height()/2+i,
                     display1.width()/2+i, display1.height()/2+i, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display1.display();
  }
}

void testdrawroundrect(void) {
  for (int16_t i=0; i<display1.height()/2-2; i+=2) {
    display1.drawRoundRect(i, i, display1.width()-2*i, display1.height()-2*i, display1.height()/4, BLACK);
    display1.display();
  }
}

void testfillroundrect(void) {
  uint8_t color = BLACK;
  for (int16_t i=0; i<display1.height()/2-2; i+=2) {
    display1.fillRoundRect(i, i, display1.width()-2*i, display1.height()-2*i, display1.height()/4, color);
    if (color == WHITE) color = BLACK;
    else color = WHITE;
    display1.display();
  }
}
   
void testdrawrect(void) {
  for (int16_t i=0; i<display1.height()/2; i+=2) {
    display1.drawRect(i, i, display1.width()-2*i, display1.height()-2*i, BLACK);
    display1.display();
  }
}

void testdrawline() {  
  for (int16_t i=0; i<display1.width(); i+=4) {
    display1.drawLine(0, 0, i, display1.height()-1, BLACK);
    display1.display();
  }
  for (int16_t i=0; i<display1.height(); i+=4) {
    display1.drawLine(0, 0, display1.width()-1, i, BLACK);
    display1.display();
  }
  delay(250);
  
  display1.clearDisplay();
  for (int16_t i=0; i<display1.width(); i+=4) {
    display1.drawLine(0, display1.height()-1, i, 0, BLACK);
    display1.display();
  }
  for (int8_t i=display1.height()-1; i>=0; i-=4) {
    display1.drawLine(0, display1.height()-1, display1.width()-1, i, BLACK);
    display1.display();
  }
  delay(250);
  
  display1.clearDisplay();
  for (int16_t i=display1.width()-1; i>=0; i-=4) {
    display1.drawLine(display1.width()-1, display1.height()-1, i, 0, BLACK);
    display1.display();
  }
  for (int16_t i=display1.height()-1; i>=0; i-=4) {
    display1.drawLine(display1.width()-1, display1.height()-1, 0, i, BLACK);
    display1.display();
  }
  delay(250);

  display1.clearDisplay();
  for (int16_t i=0; i<display1.height(); i+=4) {
    display1.drawLine(display1.width()-1, 0, 0, i, BLACK);
    display1.display();
  }
  for (int16_t i=0; i<display1.width(); i+=4) {
    display1.drawLine(display1.width()-1, 0, i, display1.height()-1, BLACK); 
    display1.display();
  }
  delay(250);
}


