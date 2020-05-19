/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */

const int SWITCH_LED = 0x4;

const int lightsPin = 9;

const initController() {
  pinMode(lightsPin, OUTPUT);
}

void executeCommand(int cmd[]) {
  switch(cmd[1]) {
    case SWITCH_LED: switchLight(cmd[3]); break;
  }
}

void switchLight(int v) {
  if(v == 0) {
    Serial.println("Switch the linght off");
  }else {
    Serial.println("Switch the linght on");
  }
  
  switchLED(v);
}

void switchLED(int value) {
    analogWrite(lightsPin, value);
}
