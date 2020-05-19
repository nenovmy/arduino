/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */

#include "SoftwareSerial.h"

const int START_COMMAND = 0x1;

SoftwareSerial mySerial(2, 3); // TX, RX

int buff[10];
int currPos = 0;

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

void readCommand() {
  while (mySerial.available()) {
    int nc = mySerial.read();
    if (currPos > 0 || nc == START_COMMAND)
      parseNext(nc);
  }
}

void parseNext(int next) {
  buff[currPos++] = next;
  if (isCommandFullyRead()) {
    executeCommand(buff);
    currPos = 0;
  }
}

bool isCommandFullyRead() {
  return currPos > 2             && // command header is 3 bytes (START, Command type, num of parameters)
         buff[2] == currPos - 3;    // are all parameters already read
}
