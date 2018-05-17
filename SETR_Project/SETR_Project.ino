#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <gfxfont.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#include "game.h"

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display1 = Adafruit_PCD8544(3, 4, 5, 7, 6);
Adafruit_PCD8544 display2 = Adafruit_PCD8544(9, 10, 11, 13, 12);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

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

  display1.begin();
  display2.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!
  display1.setContrast(50);
  display2.setContrast(50);

  display1.clearDisplay();
  display2.clearDisplay();

  display1.display();
  display2.display();

  int e1 = teste();

  

  /*for (int i=display2.width()/2; i <= display2.width()*2 + 10; i++) {
    //if (i == '\n') continue;
    if (i <= display1.width()+10){
        display1.fillCircle(i, display1.height()/2, 10, BLACK);
        display1.display();
    } 

    if (i > display1.width()-10){
      display2.clearDisplay();
    }
    
    if (i >= display1.width()-10){
        display2.fillCircle(i - display1.width(), display2.height()/2, 10, BLACK);
        display2.display();
    }
    delay(50);
    display1.clearDisplay();
    display2.clearDisplay();
  } */ 

  for (int i=display2.width()/2; i <= display2.width()*2 + 10; i++) {
    //if (i == '\n') continue;
    if (i <= display1.width()+10){
        display1.fillCircle(i, display1.height()/2, 10, BLACK);
        display1.display();
    } 

    if (i > display1.width()-10){
      display2.clearDisplay();
    }
    
    if (i >= display1.width()-10){
        display2.fillCircle(i - display1.width(), display2.height()/2, 10, BLACK);
        display2.display();
    }
    delay(50);
    display1.clearDisplay();
    display2.clearDisplay();
  }
  display1.setTextColor(WHITE, BLACK); // 'inverted' text
  display1.println(3.141592);
  display1.setTextSize(2);
  display1.setTextColor(BLACK);
  display1.print("0x"); display1.println(0xDEADBEEF, HEX);
  display1.display();
  delay(2000);
  /*display2.fillCircle(-10, display2.height()/2, 10, BLACK);
  display2.display();
  delay(2000);*/

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
  testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT);
}


void loop() {
  
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


