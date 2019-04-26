
/* 
 *  WarmCat6x14Backpack.cpp - Library for the 
 *  6 digit, 14 segment display packpack.
 *  Created April 2019.
 *  Wayne K Jones - WarmCat Solutions Ltd.
 *  Released under Creative Commons, 
 *  any reproduction must include above text.
 */

#include "Arduino.h"
#include "WarmCat6x14Backpack.h"

uint8_t WarmCat6x14::_displayCount;

WarmCat6x14::WarmCat6x14(uint8_t displayCount)
{
  _displayCount = displayCount;
}


void WarmCat6x14::init(void) {
  for (uint8_t disp = 0; disp < _displayCount; disp ++) {
    Wire.begin();
    Wire.beginTransmission(DisplayNo[disp]);
    Wire.write(0x20 | 1); // turn on oscillator
    Wire.endTransmission();
    setBrightness(15);
    blink(0);
    delay(2);
  }
}

void WarmCat6x14::clear(void) {
  for(uint8_t i = 0; i < 8; i++) {
    displayBuffer[i] = 0;
  }
  for(uint8_t disp=0; disp < _displayCount; disp++) {
    showOnDisp(disp);
  }
}

void WarmCat6x14::blink(uint8_t bl) {
  if ((bl < 0)|(bl > 3)) { bl = 0; }  // turn off if invalid value
  for (uint8_t disp=0; disp< _displayCount; disp++) {  //iterate through displays and set them all
      Wire.beginTransmission(DisplayNo[disp]);
      Wire.write(0x80 | bl << 1 | 1); // blinking/blank command
      Wire.endTransmission();
  }
}

void WarmCat6x14::setBrightness(uint8_t brightness) {
  for (uint8_t disp=0; disp< _displayCount; disp++) {  //iterate through displays and set them all
      Wire.beginTransmission(DisplayNo[disp]);
      Wire.write(0xE0 | brightness); // Dimming command
      Wire.endTransmission();
  }
}

void WarmCat6x14::showOnDisp(uint8_t disp) {
  Wire.beginTransmission(DisplayNo[disp]);
  //Serial.print("showOnDisp : ");
  //Serial.println(DisplayNo[disp]);
  Wire.write(0x00); // start at address 0x0
 
  for (int i = 0; i < 8; i++) {
    Wire.write(displayBuffer[i]);   
    Wire.write(displayBuffer[i] >> 8);    
  }
  Wire.endTransmission();  
}

void WarmCat6x14::showScroll(void) {
  for (int disp = 0; disp< _displayCount; disp++) {
    Wire.beginTransmission(DisplayNo[disp]);
    Wire.write(0x00); // start at address 0x0
    for (int i = 0; i < 8; i++) {
      // Wire.write(displayBuffer[i] & 0xFF);   //why? in case byte is less than 8 bits?
      Wire.write(scrollBuffer[disp][i]);   
      Wire.write(scrollBuffer[disp][i] >> 8);    
    }
  Wire.endTransmission();  
  }
}

void WarmCat6x14::emptyScrollBuffer(void) {
  for (int x = 0; x < _displayCount; x++) {
    memset(scrollBuffer[x], 0, sizeof(scrollBuffer));
  }
}

void WarmCat6x14::dots(void) {
  clear();
  for (uint8_t disp=0; disp < _displayCount; disp++) {
    for (int digit=5; digit>=0; digit--) {
      displayBuffer[digit] = 0b100000000000000;
      showOnDisp(disp);
      delay(100);
    }
      memset(displayBuffer, 0, sizeof(displayBuffer));
  }
}

void WarmCat6x14::disp6Char(char text[], uint8_t disp) {
  for (uint8_t x=0;x<6;x++) {
    displayBuffer[x] = FourteenSegmentASCII[text[x]-32];
  }
  showOnDisp(disp);
}

void WarmCat6x14::scrollText(char text[], int scrollrate) {
  bool testText = false;
  emptyScrollBuffer();  //empty the scrollBuffer
  int textLen = strlen(text);  //get length of string
  //Test shit
  if (testText) {
    Serial.print("Scrolling Text: '");
    Serial.print(text);
    Serial.println("'");
    Serial.print("String length: ");
    Serial.println(textLen);
  }
  //end of test shit
  clear(); //Clear Display first
  //for each character in string
  for (int i=0; i<=textLen-1; i++) {
    // for each display backpack
    for (int disp = _displayCount; disp >= 0; disp--) {
      //Serial.print(scrollBuffer[disp][0]);
      scrollBuffer[disp][0]=scrollBuffer[disp][1];
      scrollBuffer[disp][1]=scrollBuffer[disp][2];
      scrollBuffer[disp][2]=scrollBuffer[disp][3];
      scrollBuffer[disp][3]=scrollBuffer[disp][4];
      scrollBuffer[disp][4]=scrollBuffer[disp][5];
      if (disp==0) {
        // get next character from the buffer if this is the first backpack
        scrollBuffer[disp][5]=FourteenSegmentASCII[text[i]-32];
      } else {
        // else shift from the previous display backpack
        scrollBuffer[disp][5]=scrollBuffer[disp-1][0];
      }
    }
    if (testText) {
      Serial.print(text[i]);
      if (i<textLen-1) { Serial.print("-"); }
    }
    delay(scrollrate);
    showScroll();
  }
  Serial.println("");
  Serial.println("Done!");
  delay(2000);
}

void WarmCat6x14::dispChar(uint8_t disp, uint8_t digit, byte ascii, bool dp) {
  byte index = ascii - 32;
  displayBuffer[digit] = FourteenSegmentASCII[index];
  if (dp) { displayBuffer[digit] |= (1<<14); }
  showOnDisp(disp);
}

void WarmCat6x14::swirlyAll(int swirlrate) {
  clear();
  for (int x = 0 ; x < 14 ; x++) {
    for (int d=0; d<6; d++) {
      displayBuffer[d] |= Swirly[x];
    }
    for (int disp=0; disp<_displayCount; disp++) {
      showOnDisp(disp);
    }
    delay(swirlrate);
  }
  delay(2*swirlrate);
}

void WarmCat6x14::swirly(int swirlrate) {
  clear();
  for (int disp=0; disp<_displayCount; disp++) {
    for (int d=5; d>=0; d--) {
      for (int x=0; x<14; x++) {
        displayBuffer[d] |= Swirly[x];
        showOnDisp(disp);
        delay(swirlrate);
      }
    }
    memset(displayBuffer, 0, sizeof(displayBuffer));
  }
}
