/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */
#include "Arduino.h"
#include "bipolarstepper.h"

//BiPolStepper s1 = BiPolStepper(A0, A1, A2, A3, 2048);
//BiPolStepper s2 = BiPolStepper(10, 11, A4, A5, 2048);
BiPolStepper s3 = BiPolStepper(2, 3, 4, 5, 2048);
//BiPolStepper s4 = BiPolStepper(6, 7, 8, 9, 2048);

bool dir = false;


void setup() {
//  s1.rotateFull(10, false);
//  s2.rotateFull(10, true);
  s3.rotateFull(12, dir);
//  s4.rotateFull(10, true);
}

void loop() {
//  s1.update();
//  s2.update();
  s3.update();
  if(!s3.isRunning()) {
    dir = !dir;
    s3.rotateFull(12, dir);    
  }
//  s4.update();
}
