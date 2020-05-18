//#include "Arduino.h"
#include "SoftwareSerial.h"
//#include "HardwareSerial.h"

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
