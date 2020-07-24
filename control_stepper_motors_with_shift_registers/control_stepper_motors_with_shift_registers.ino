/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */
#include "Arduino.h"
#include "bipolarstepper.h"

#define DATA_PIN 5    // D1
#define CLOCK_PIN 4   // D2
#define LATCH_PIN 14  // D5

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = LATCH_PIN;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = CLOCK_PIN;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = DATA_PIN;

BiPolStepper s[] = {BiPolStepper(2048), BiPolStepper(2048), BiPolStepper(2048), BiPolStepper(2048)};

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  s[0].rotateFull(100, true);
  s[1].rotateFull(100, false);
  s[2].rotateFull(100, true);
  s[3].rotateFull(100, false);
}

void loop() {
  updateAllStepper();
  if(areStepperChanged()) {
    setNewStepperState();
    clearChangedState();
  }
}

void updateAllStepper() {
  for(int i=0; i<4; i++)
    s[i].update();
}

bool areStepperChanged() {
  for(int i=0; i<4; i++) {
    if(s[i].isUpdated) {
      return true;
    }
  }

  return false;
}

void setNewStepperState() {
  digitalWrite(latchPin, LOW); 

  shiftOut(dataPin, clockPin, MSBFIRST, stepperStateToByte(s[0], s[1]));
  shiftOut(dataPin, clockPin, MSBFIRST, stepperStateToByte(s[2], s[3]));
  
  digitalWrite(latchPin, HIGH);
}

void clearChangedState() {
  for(int i=0; i<4; i++)
    s[i].clearUpdated();
}

int stepperStateToByte(BiPolStepper s1, BiPolStepper s2) {
  int b = 0;

  for(int i=0; i<4; i++) {
    b |= s1.state[i] << i;
    b |= s2.state[i] << (i + 4);
  }

  return b;
}
