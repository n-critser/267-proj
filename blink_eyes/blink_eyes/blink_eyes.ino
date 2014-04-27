/*************************************************** 
  This is a library for our I2C LED Backpacks

  Designed specifically to work with the Adafruit LED Matrix backpacks 
  ----> http://www.adafruit.com/products/872
  ----> http://www.adafruit.com/products/871
  ----> http://www.adafruit.com/products/870

  These displays use I2C to communicate, 2 pins are required to 
  interface. There are multiple selectable I2C addresses. For backpacks
  with 2 Address Select pins: 0x70, 0x71, 0x72 or 0x73. For backpacks
  with 3 Address Select pins: 0x70 thru 0x77

  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
  
  UNO PINS ----------------------------------------------
  
  analog #5 = CLK
  analog #4 = DAT
   
 ****************************************************/

#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"

Adafruit_8x8matrix matrixLeft = Adafruit_8x8matrix();
Adafruit_8x8matrix matrixRight = Adafruit_8x8matrix();

void setup() {
  Serial.begin(9600);
  digitalWrite(19, HIGH); //set digital13 to high 

  Serial.println("8x8 LED Matrix Test");
  
  matrixLeft.begin(0x70);  // pass in the address
  matrixRight.begin(0x71);
}

static const uint8_t PROGMEM
  open_leftEye_bmp[] =
  { B00111100,
    B01000100,
    B10011010,
    B10100101,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
    open_rightEye_bmp[] =
  { B00111100,
    B01000010,
    B10011001,
    B10100101,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
    
   closeEye_bmp[] =
  { B00000000,
    B00000000,
    B00110000,
    B01000100,
    B11111111,
    B00000000,
    B00000000,
    B00000000 },
    slit_bmp[] =
    {B00000000,
    B00000000,
    B00000000,
    B00000000,
    B11111111,
    B00000000,
    B00000000,
    B00000000 },
      
 slant_rightEye_bmp[] =
  { B10000000,
    B00100000,
    B00001000,
    B00010100,
    B00100010,
    B01000111,
    B00101110,
    B00011100 };

void loop() {
  matrixRight.clear();
  matrixRight.drawBitmap(0, 0, open_rightEye_bmp, 8, 8, LED_ON);
  matrixRight.writeDisplay();
  delay(500);

  matrixLeft.clear();
  matrixLeft.drawBitmap(0, 0, open_leftEye_bmp, 8, 8, LED_ON);
  matrixLeft.writeDisplay();
  delay(500);

  matrixRight.clear();
  matrixRight.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
  matrixRight.writeDisplay();
  delay(500);
  
  matrixLeft.clear();
  matrixLeft.drawBitmap(0, 0, closeEye_bmp, 8, 8, LED_ON);
  matrixLeft.writeDisplay();
  delay(500);

  matrixLeft.clear();
  matrixLeft.drawBitmap(0, 0, slit_bmp, 8, 8, LED_ON);
  matrixLeft.writeDisplay();
  delay(500);
  
 
  matrixRight.clear();
  matrixRight.drawBitmap(0,0, slant_rightEye_bmp, 8, 8, LED_ON);
  matrixRight.writeDisplay();
  delay(500);

/*
  matrix.clear();
  matrix.drawLine(4,0, 4,7, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);
*/

/*
  matrix.clear();
  matrix.drawCircle(3,3, 3, LED_ON);
  matrix.writeDisplay();  // write the changes we just made to the display
  delay(500);
  
*/  
/*
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  for (int8_t x=0; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("FIXME");
    matrix.writeDisplay();
    delay(100);
  }
  matrix.setRotation(3);
  for (int8_t x=7; x>=-36; x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print("00");
    matrix.writeDisplay();
    delay(100);
  }
  
  */
  matrixLeft.setRotation(0);
  matrixRight.setRotation(0);
}
