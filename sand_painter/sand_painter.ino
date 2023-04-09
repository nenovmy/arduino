/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */
#include "Arduino.h"
#include "bipolarstepper.h"
#include "delta_robot.h"

#define DATA_PIN 5    // D1
#define CLOCK_PIN 4   // D2
#define LATCH_PIN 0//14  // D5
#define RELLAY_PIN 2  // D4

int SWITCH_1 = 13; // M1
int LED_1 = 16;
int SWITCH_2 = 12;
int LED_2 = 3;
int SWITCH_3 = 14;
int LED_3 = 15;

//Pin connected to latch pin (ST_CP) of 74HC595
const int latchPin = LATCH_PIN;
//Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = CLOCK_PIN;
////Pin connected to Data in (DS) of 74HC595
const int dataPin = DATA_PIN;

DeltaRobot * dr;

int controlWord = 0;
bool isMovingUp = true;

void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("Setup start!");
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  pinMode(RELLAY_PIN, OUTPUT);
  digitalWrite(RELLAY_PIN, HIGH );

  pinMode(LED_1 , OUTPUT);
  pinMode(SWITCH_1, INPUT_PULLUP);
  pinMode(LED_2 , OUTPUT);
  pinMode(SWITCH_2, INPUT_PULLUP);
  pinMode(LED_3 , OUTPUT);
  pinMode(SWITCH_3, INPUT_PULLUP);

  dr = new DeltaRobot(new BiPolStepper(2048, 10.), new BiPolStepper(2048, 10.), new BiPolStepper(2048, 10.));

  initZeroState();
//  controlWord =  0;
  setNewStepperState();
  delay(1000);
//  controlWord =  1;
  setNewStepperState();

  digitalWrite(RELLAY_PIN, LOW );

  Serial.println("Setup finished");
}

void loop() {
  dr->updateRobot();

  if(areStepperChanged()) {
    setNewStepperState();
    clearChangedState();
    updateLimitSwitches();
  }
}

void updateLimitSwitches() {
  bool ls1 = digitalRead(SWITCH_1) == HIGH;
  bool ls2 = digitalRead(SWITCH_2) == HIGH;
  bool ls3 = digitalRead(SWITCH_3) == HIGH;
  digitalWrite(LED_1, ls1?HIGH:LOW);
  digitalWrite(LED_2, ls2?HIGH:LOW);
  digitalWrite(LED_3, ls3?HIGH:LOW);
  dr->updateLimitSwitches(ls1, ls2, ls3);
}

bool areStepperChanged() {
  return dr->s1->isUpdated || dr->s2->isUpdated || dr->s3->isUpdated;
}

void initZeroState() {
  digitalWrite(latchPin, LOW); 

  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  shiftOut(dataPin, clockPin, MSBFIRST, 0);
  
  digitalWrite(latchPin, HIGH);
}

void setNewStepperState() {
  digitalWrite(latchPin, LOW); 

  shiftOut(dataPin, clockPin, MSBFIRST, stepperStateToByte(dr->s3));
  shiftOut(dataPin, clockPin, MSBFIRST, stepperStateToByte(dr->s1, dr->s2));
  
  digitalWrite(latchPin, HIGH);
}

void clearChangedState() {
  dr->s1->clearUpdated();
  dr->s2->clearUpdated();
  dr->s3->clearUpdated();
}

int stepperStateToByte(BiPolStepper * s1) {
  int b = 0;

  for(int i=0; i<4; i++) {
    b |= s1->state[i] << i;
    b |= controlWord << 4;
  }

  return b;
}

int stepperStateToByte(BiPolStepper * s1, BiPolStepper * s2) {
  int b = 0;

  for(int i=0; i<4; i++) {
    b |= s1->state[i] << i;
    b |= s2->state[i] << (i + 4);
  }

  return b;
}
