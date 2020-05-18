/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */

#include "SoftwareSerial.h"

SoftwareSerial mySerial(2, 3); // TX, RX

void initBluetooth() {
  mySerial.begin(9600);
  Serial.begin(9600);
}

// Used for manual setup
void updateSerial() {
  if (mySerial.available())       
   Serial.write(mySerial.read()); 

  if (Serial.available())     
    mySerial.write(Serial.read());
}
