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

  sendCommand("AT");
  sendCommand("AT+ROLE0");
  sendCommand("AT+UUIDFFE0");
  sendCommand("AT+CHARFFE1");
  sendCommand("AT+NAMEBlueCArd");
}

void sendCommand(const char * command) {
  Serial.print("Command send :");
  Serial.println(command);
  mySerial.println(command);
  //wait some time
  delay(100);

  char reply[100];
  int i = 0;
  while (mySerial.available()) {
    reply[i] = mySerial.read();
    i += 1;
  }
  //end the string
  reply[i] = '\0';
  Serial.print(reply);
  Serial.println("\nReply end");
  delay(100);
}

// Used for manual setup
void updateSerial() {
  if (mySerial.available())       
   Serial.write(mySerial.read()); 

  if (Serial.available())     
    mySerial.write(Serial.read());
}
