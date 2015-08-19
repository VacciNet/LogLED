#include "LPD8806.h"
#include "SPI.h"
#include <Time.h>
#include <string.h>
// library here: http://arduino.cc/playground/Code/Time 

// Example to control LPD8806-based RGB LED Modules in a strip!
// NOTE: WILL NOT WORK ON TRINKET OR GEMMA due to floating-point math
/*****************************************************************************/

int dataPin = 2;
int clockPin = 1;

// Set the first variable to the NUMBER of pixels. 32 = 32 pixels in a row
// The LED strips are 32 LEDs per meter but you can extend/cut the strip
LPD8806 strip = LPD8806(32, dataPin, clockPin);

int ledPin = 11; 
int ExternalLED = 20; //connect your test LED to pin 20 on Teensy 
int ModeZ; 
int PSCmd; 
char c; 
time_t pctime;  
boolean TimeisSet; 
long SecsStart, SecsEnd, SecsElapsed; 

 // LED function prototypes, do not remove these!
void colorChase(uint32_t c, uint8_t wait);
void colorWipe(uint32_t c, uint8_t wait);
void dither(uint32_t c, uint8_t wait);
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait);
void wave(uint32_t c, int cycles, uint8_t wait);
void rainbowCycle(uint8_t wait);
uint32_t Wheel(uint16_t WheelPos);

//----------------------------------------------------------------------------  
 
void setup()  
{ 
  // Start up the LED strip
  strip.begin();

  // Update the strip, to start they are all 'off'
  strip.show();

  digitalWrite(ledPin, HIGH); // set the onboard LED on       
  Serial.begin(9600);    
  pinMode(ExternalLED, OUTPUT);      
  delay(2000); 
} 
 
//----------------------------------------------------------------------------  
bool Flash = 0;
uint8_t flashr = 0;
uint8_t flashg = 0;
uint8_t flashb = 0;
uint8_t flashled = 0;
  

void loop()  
{ 
  uint8_t* ModeZ;
  ModeZ = GetMode(); //retrieve LED codes(Led1,Led2,Led3,Led4,Led5,Led6,Led7,Led8,Led9,Led10,Led11,Le12,Led13,Led14,Led15,Led16,Led17,Led18,Led19,Led20,Led21,Led22,Led23,Led24,Led25,Led26,Led27,Led28,Led29,Led30,Led31,Led32,Blink,Effect)
  int codePosition = 2;
  if (ModeZ[1] == 6)
  {
if (ModeZ[99] > 0)
  {
    if (ModeZ[99] == 9) 
    { 
      Serial.println(F("*** Teensy functions programmed on this board ***")); 
      Serial.println(F("1 - Chase")); 
      Serial.println(F("2 - Fill")); 
      Serial.println(F("3 - Sparkles")); 
      Serial.println(F("4 - Christmas Sparkles")); 
      Serial.println(F("5 - Cops")); 
      Serial.println(F("6 - CandyCane")); 
      Serial.println(F("7 - Icy")); 
      Serial.println(F("8 - Rainbow")); 
      Serial.println(F("9 - This Menu")); 
      Serial.println(F("@ - Send RGB codes for each led plus index to blink")); 
    } 
    if (ModeZ[99] == 1) 
    { 
      Serial.println(F("Color Chase")); 
      clear();
      colorChase(strip.Color(127,127,127), 20); // white
      colorChase(strip.Color(127,0,0), 20);     // red
      colorChase(strip.Color(127,127,0), 20);   // yellow
      colorChase(strip.Color(0,127,0), 20);     // green
      colorChase(strip.Color(0,127,127), 20);   // cyan
      colorChase(strip.Color(0,0,127), 20);     // blue
      colorChase(strip.Color(127,0,127), 20);   // magenta
    }
   
    if (ModeZ[99] == 2) 
    {  
      Serial.println(F("Color Fill")); 
      clear();
      colorWipe(strip.Color(127,127,127), 20); // white
      colorWipe(strip.Color(127,0,0), 20);     // red
      colorWipe(strip.Color(127,127,0), 20);   // yellow
      colorWipe(strip.Color(0,127,0), 20);     // green
      colorWipe(strip.Color(0,127,127), 20);   // cyan
      colorWipe(strip.Color(0,0,127), 20);     // blue
      colorWipe(strip.Color(127,0,127), 20);   // magenta  
    } 
      
    if (ModeZ[99] == 3) 
    { 
      Serial.println(F("Color Sparkles-cyan-slow/black-fast/magenta-slow/black-fast/yellow-slow/black-fast")); 
      // Color sparkles
      clear();
      dither(strip.Color(0,127,127), 50);       // cyan, slow
      dither(strip.Color(0,0,0), 15);           // black, fast
      dither(strip.Color(127,0,127), 50);       // magenta, slow
      dither(strip.Color(0,0,0), 15);           // black, fast
      dither(strip.Color(127,127,0), 50);       // yellow, slow
      dither(strip.Color(0,0,0), 15);           // black, fast
    } 
   
    if (ModeZ[99] == 4) 
    { 
      Serial.println(F("Christmas Sparkles-green-slow/red-slow/green-slow/red-fast")); 
      clear();
      // Color sparkles
      dither(strip.Color(0,127,0), 50);           // green, slow
      dither(strip.Color(127,0,0), 50);           // red, slow
      dither(strip.Color(0,127,0), 15);           // green, fast
      dither(strip.Color(127,0,0), 15);           // red, fast
    } 
      
    if (ModeZ[99] == 5) 
    { 
      Serial.println(F("Back and Forth Lights")); 
      clear();    
      // Back-and-forth lights
      for (int i = 30; i > 0; i =i - 15)
      { 
      scanner(127,0,0, i);        // red, slow
      scanner(0,0,127, i);        // blue, fast
      }
    } 
     
    if (ModeZ[99] == 6) 
    { 
      Serial.println(F("Candy Cane")); 
      clear();    
      // Wavy ripple effects
      wave(strip.Color(127,0,0), 4, 20);        // candy cane
    } 
      
    if (ModeZ[99] == 7) 
    { 
      Serial.println(F("Icy")); 
      clear();    
      // Wavy ripple effects
      wave(strip.Color(0,0,100), 1, 40);        // icy
    } 
      
    if (ModeZ[99] == 8) 
    { 
      Serial.println(F("Rainbow")); 
      clear();    
      // make a rainbow.
      rainbowCycle(0);  // make it go through the cycle fairly fast  
    }
    if (ModeZ[99] == 9) 
    { 
      Serial.println(F("Emergency")); 
      clear();    
      // Danger!
      cops(50);  // 50ms delay
    }
 
    ModeZ[99] = 0;
  }
  
  ModeZ[1] = 0;
    //Flash the provided LED
    flashled = ModeZ[98];
      
    for (int i = 0; i < strip.numPixels() + 1; i++)
    {
      strip.setPixelColor(i, 0);
      strip.show();              // refresh strip display
    }
    for (int i = 0; i < strip.numPixels() + 1; i++)
    {
      strip.setPixelColor(i, ModeZ[codePosition], ModeZ[codePosition + 1], ModeZ[codePosition + 2]);
      strip.show();              // refresh strip display
      delay(5);               // hold image for a moment
      codePosition = codePosition + 3;      
    }

  }
  if (Flash == 0)
      { 
        uint8_t flashstartpos = ((flashled -1) * 3) + 2;
        flashr = ModeZ[flashstartpos];
        flashg = ModeZ[flashstartpos + 1];
        flashb = ModeZ[flashstartpos + 2];
        strip.setPixelColor(flashled, 0,0,0);
        strip.show();
        delay(50);
        Flash = 1;
      }
      else
      {
        strip.setPixelColor(flashled, flashr,flashg,flashb);
        strip.show();
        delay(50);
        Flash = 0;
      }

  
} 
 
//----------------------------------------------------------------------------  
 
uint8_t* GetMode() 
{ 
  //clear read buffer 
  while (Serial.available()){ Serial.read();} 
   
  //attempt to read serial port for 2 seconds 
  SecsStart = now();
  uint8_t codes[390];
  uint8_t flash;
  do        
  {    
    if (Serial.available()) 
    {  
      c = Serial.read(); 
      if (c == '@') 
      { 
        int d = 1;
        int code = 0;
        codes[1] = 6;
        
        for (int i = 1; i < 391; i++) 
        {    
          c = Serial.read();
          int ic = c - '0';
          if (!(c == ','))
            code = (10 * code) + ic;  
          else
          {
            d++;
            codes[d] = code;
            code = 0;
          }
        } 
        return codes;
      } 
      //clear read buffer 
      while (Serial.available()){ Serial.read();} 
    } 
 
    //break loop after 2 secs 
    SecsEnd = now(); 
    SecsElapsed = SecsEnd-SecsStart;  
 
  }while(SecsElapsed<2);  
   
  return codes;   
} 
 
//----LED Functions------------------------------------------------------------------------  
 
void clear() {
  // Clear strip data before start of next effect
  for (int i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);
  }
}

// Alternate red and blue flashes
void cops(int dly) {
  int quarter = strip.numPixels() / 4;
  // Triple flash red and blue in this pattern
  // RRRRRRRR........RRRRRRRR........
  // ........BBBBBBBB........BBBBBBBB
  for (int s = 0; s < 4; s++) {
    for (int r = 0; r < 3; r++){
      for (int i = 0; i < (quarter); i++){
        strip.setPixelColor(i, 127,0,0);
      }
      for (int i = (quarter * 2); i < (quarter * 3); i++){
        strip.setPixelColor(i, 127,0,0);
      }
      strip.show();
      delay(dly);
      
      for (int i = 0; i < strip.numPixels(); i++){
        strip.setPixelColor(i, 0, 0,0);   
      }
      strip.show();
      delay(dly);
    }

    for (int r = 0; r < 3; r++){
      for (int i = quarter; i < (quarter * 2); i++){
        strip.setPixelColor(i, 0, 0,127);
      }
      for (int i = (quarter * 3); i < strip.numPixels(); i++){
        strip.setPixelColor(i, 0, 0,127);
      }
      strip.show();
      delay(dly);
  
      for (int i = 0; i < strip.numPixels(); i++){
        strip.setPixelColor(i, 0, 0,0);   
      }
      strip.show();  
      delay(dly);
    }
  }
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR strobe 3 times
    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB strobe 3 times
    // Repeat 2 more times - half speed
    for (int s = 0; s < 3; s++){
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 127,0,0);
        }
        strip.show();
        delay(dly * 2);
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly * 2);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0,0,127);
        }
        strip.show();
        delay(dly * 2);
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly * 2);
      }
    }
    // Flash red and blue back and forth in this pattern
    // RRRRRRRRRRRRRRRR...............
    // ................BBBBBBBBBBBBBBB
    // Then switch
    // BBBBBBBBBBBBBBBB...............
    // ................RRRRRRRRRRRRRRR
    for (int r = 0; r < 4; r++) {
      for (int i = 0; i < (quarter * 2); i++) {
        strip.setPixelColor(i, 127, 0, 0);
      }
      for (int i = (quarter * 2); i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.show();
      delay(dly * 2);

      for (int i = 0; i < (quarter * 2); i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      for (int i = (quarter * 2); i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0, 0, 127);
      }
      strip.show();
      delay(dly * 2);  
    }
    for (int r = 0; r < 6; r++) {
      for (int i = 0; i < (quarter * 2); i++) {
        strip.setPixelColor(i, 0, 0, 127);
      }
      for (int i = (quarter * 2); i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      strip.show();
      delay(dly * 2);

      for (int i = 0; i < (quarter * 2); i++) {
        strip.setPixelColor(i, 0, 0, 0);
      }
      for (int i = (quarter * 2); i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 127, 0, 0);
      }
      strip.show();
      delay(dly * 2);  
    }

    // Double flash red and blue in this pattern
    // RR..RR..RR..RR..RR..RR..RR..RR.. strobe 3 times
    // ..BB..BB..BB..BB..BB..BB..BB..BB strobe 3 times
    // Repeat 3 times
    for (int s = 0; s < 4; s++) {  
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < strip.numPixels(); i = i + 4){
          strip.setPixelColor(i, 127,0,0);
          strip.setPixelColor(i + 1, 127,0,0);
        }
        strip.show();
        delay(dly * 1.5);
        for (int i = 0; i < strip.numPixels(); i = i + 4){
          strip.setPixelColor(i, 0, 0,0);   
          strip.setPixelColor(i + 1, 0, 0,0);
        }
        strip.show();
        delay(dly);
      }

      for (int r = 0; r < 3; r++){
        for (int i = 2; i < strip.numPixels(); i = i + 4){
          strip.setPixelColor(i, 0, 0,127);
          strip.setPixelColor(i + 1, 0, 0,127);
        }
        strip.show();
        delay(dly * 1.5);
    
        for (int i = 2; i < strip.numPixels(); i = i + 4){
          strip.setPixelColor(i, 0, 0,0);
          strip.setPixelColor(i + 1, 0, 0,0);
        }
        strip.show();  
        delay(dly);
      }
    }

    // Use whole strip switching back and forth red to blue 
    // first solid then strobing more and more
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR
    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
    // Quarter speed
    for (int r = 0; r < 2; r++) {
      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 127, 0, 0);
      }
      strip.show();
      delay(dly * 4);

      for (int i = 0; i < strip.numPixels(); i++) {
        strip.setPixelColor(i, 0, 0, 127);
      }
      strip.show();
      delay(dly * 4);
    }

    // RRRRRRRRRRRRRRRRRRRRRRRRRRRRRRRR strobe 6 times
    // BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB strobe 6 times
    // Repeat 1 once - full speed
    for (int s = 0; s < 2; s++){
      for (int r = 0; r < 6; r++){
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 127,0,0);
        }
        strip.show();
        delay(dly);
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 6; r++){
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0,0,127);
        }
        strip.show();
        delay(dly);
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    }

    // RRRRRRRR................BBBBBBBB strobe 3 times
    // ........RRRRRRRRBBBBBBBB........ strobe 3 times
    // repeat 4 times full speed
    for (int s = 0; s < 4; s++){
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < quarter; i++){
          strip.setPixelColor(i, 127,0,0);
        }
        for (int i = (quarter * 3); i < strip.numPixels(); i++){
          strip.setPixelColor(i,0,0,127);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = quarter; i < (quarter * 2); i++){
          strip.setPixelColor(i, 127,0,0);
        }
        for (int i = (quarter * 2); i < (quarter * 3); i++) {
          strip.setPixelColor(i, 0,0,127);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    }

    // BBBBBBBB................BBBBBBBB strobe 3 times
    // ........WWWWWWWWWWWWWWWW........ strobe 3 times
    // WWWWWWWW................WWWWWWWW strobe 3 times
    // ........BBBBBBBBBBBBBBBB........ strobe 3 times
    // RRRRRRRR................RRRRRRRR strobe 3 times
    // ........WWWWWWWWWWWWWWWW........ strobe 3 times
    // WWWWWWWW................WWWWWWWW strobe 3 times
    // ........RRRRRRRRRRRRRRRR........ strobe 3 times
    // repeat once full speed
    for (int s = 0; s < 2; s++){
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < quarter; i++){
          strip.setPixelColor(i, 0,0,127);
        }
        for (int i = (quarter * 3); i < strip.numPixels(); i++){
          strip.setPixelColor(i,0,0,127);
        }
        strip.show();
        delay(dly*1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = quarter; i < (quarter * 3); i++){
          strip.setPixelColor(i, 127,127,127);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < quarter; i++){
          strip.setPixelColor(i, 127,127,127);
        }
        for (int i = (quarter * 3); i < strip.numPixels(); i++){
          strip.setPixelColor(i,127,127,127);
        }
        strip.show();
        delay(dly*1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = quarter; i < (quarter * 3); i++){
          strip.setPixelColor(i, 0,0,127);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < quarter; i++){
          strip.setPixelColor(i, 127,0,0);
        }
        for (int i = (quarter * 3); i < strip.numPixels(); i++){
          strip.setPixelColor(i,127,0,0);
        }
        strip.show();
        delay(dly*1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = quarter; i < (quarter * 3); i++){
          strip.setPixelColor(i, 127,127,127);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
      
      for (int r = 0; r < 3; r++){
        for (int i = 0; i < quarter; i++){
          strip.setPixelColor(i, 127,127,127);
        }
        for (int i = (quarter * 3); i < strip.numPixels(); i++){
          strip.setPixelColor(i,127,127,127);
        }
        strip.show();
        delay(dly*1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    
      for (int r = 0; r < 3; r++){
        for (int i = quarter; i < (quarter * 3); i++){
          strip.setPixelColor(i, 127,0,0);
        }
        strip.show();
        delay(dly *1.5);
        
        for (int i = 0; i < strip.numPixels(); i++){
          strip.setPixelColor(i, 0, 0, 0);   
        }
        strip.show();
        delay(dly);
      }
    }
}

// Cycle through the color wheel, equally spaced around the belt
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color
      // wheel (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel(((i * 384 / strip.numPixels()) + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

// fill the dots one after the other with said color
// good for testing purposes
void colorWipe(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

// Chase a dot down the strip
void colorChase(uint32_t c, uint8_t wait) {
  int i;

  for (i=0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, 0);  // turn all pixels off
  }

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c); // set one pixel
      strip.show();              // refresh strip display
      delay(wait);               // hold image for a moment
      strip.setPixelColor(i, 0); // erase pixel (but don't refresh yet)
  }
  strip.show(); // for last erased pixel
}

// An "ordered dither" fills every pixel in a sequence that looks
// sparkly and almost random, but actually follows a specific order.
void dither(uint32_t c, uint8_t wait) {

  // Determine highest bit needed to represent pixel index
  int hiBit = 0;
  int n = strip.numPixels() - 1;
  for(int bit=1; bit < 0x8000; bit <<= 1) {
    if(n & bit) hiBit = bit;
  }

  int bit, reverse;
  for(int i=0; i<(hiBit << 1); i++) {
    // Reverse the bits in i to create ordered dither:
    reverse = 0;
    for(bit=1; bit <= hiBit; bit <<= 1) {
      reverse <<= 1;
      if(i & bit) reverse |= 1;
    }
    strip.setPixelColor(reverse, c);
    strip.show();
    delay(wait);
  }
  delay(250); // Hold image for 1/4 sec
}

// "Larson scanner" = Cylon/KITT bouncing light effect
void scanner(uint8_t r, uint8_t g, uint8_t b, uint8_t wait) {
  int i, j, pos, dir;

  pos = 0;
  dir = 1;

  for(i=0; i<((strip.numPixels()-1) * 8); i++) {
    // Draw 5 pixels centered on pos.  setPixelColor() will clip
    // any pixels off the ends of the strip, no worries there.
    // we'll make the colors dimmer at the edges for a nice pulse
    // look
    strip.setPixelColor(pos - 2, strip.Color(r/4, g/4, b/4));
    strip.setPixelColor(pos - 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos, strip.Color(r, g, b));
    strip.setPixelColor(pos + 1, strip.Color(r/2, g/2, b/2));
    strip.setPixelColor(pos + 2, strip.Color(r/4, g/4, b/4));

    strip.show();
    delay(wait);
    // If we wanted to be sneaky we could erase just the tail end
    // pixel, but it's much easier just to erase the whole thing
    // and draw a new one next time.
    for(j=-2; j<= 2; j++) 
        strip.setPixelColor(pos+j, strip.Color(0,0,0));
    // Bounce off ends of strip
    pos += dir;
    if(pos < 0) {
      pos = 1;
      dir = -dir;
    } else if(pos >= strip.numPixels()) {
      pos = strip.numPixels() - 2;
      dir = -dir;
    }
  }
}

// Sine wave effect
#define PI 3.14159265
void wave(uint32_t c, int cycles, uint8_t wait) {
  float y;
  byte  r, g, b, r2, g2, b2;

  // Need to decompose color into its r, g, b elements
  g = (c >> 16) & 0x7f;
  r = (c >>  8) & 0x7f;
  b =  c        & 0x7f; 

  for(int x=0; x<(strip.numPixels()*5); x++)
  {
    for(int i=0; i<strip.numPixels(); i++) {
      y = sin(PI * (float)cycles * (float)(x + i) / (float)strip.numPixels());
      if(y >= 0.0) {
        // Peaks of sine wave are white
        y  = 1.0 - y; // Translate Y to 0.0 (top) to 1.0 (center)
        r2 = 127 - (byte)((float)(127 - r) * y);
        g2 = 127 - (byte)((float)(127 - g) * y);
        b2 = 127 - (byte)((float)(127 - b) * y);
      } else {
        // Troughs of sine wave are black
        y += 1.0; // Translate Y to 0.0 (bottom) to 1.0 (center)
        r2 = (byte)((float)r * y);
        g2 = (byte)((float)g * y);
        b2 = (byte)((float)b * y);
      }
      strip.setPixelColor(i, r2, g2, b2);
    }
    strip.show();
    delay(wait);
  }
}

/* Helper functions */

//Input a value 0 to 384 to get a color value.
//The colours are a transition r - g - b - back to r

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128; // red down
      g = WheelPos % 128;       // green up
      b = 0;                    // blue off
      break;
    case 1:
      g = 127 - WheelPos % 128; // green down
      b = WheelPos % 128;       // blue up
      r = 0;                    // red off
      break;
    case 2:
      b = 127 - WheelPos % 128; // blue down
      r = WheelPos % 128;       // red up
      g = 0;                    // green off
      break;
  }
  return(strip.Color(r,g,b));
}
