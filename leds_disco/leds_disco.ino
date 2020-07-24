#define DATA_PIN 5    // D1
#define CLOCK_PIN 4   // D2
#define LATCH_PIN 14  // D5

const int latchPin = LATCH_PIN;
const int clockPin = CLOCK_PIN;
const int dataPin  = DATA_PIN;

byte bits [16];

void setup() {
  //set pins to output so you can control the shift register
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

void loop() {
  while(true) {
    for(int i=0; i<3; i++)
      runningLight();
    for(int i=0; i<3; i++)
      fillAndEmpty();
    for(int i=0; i<3; i++)
      outIn();
  }
}

void runningLight() {
    for(int i=0; i<16; i++) {
      clearBits();
      bits[i] = 1;
      writeData();
      // pause before next value:
      delay(25);
    }
    for(int i=15; i>=0; i--) {
      clearBits();
      bits[i] = 1;
      writeData();
      // pause before next value:
      delay(25);
    }  
}

void fillAndEmpty() {
    clearBits();
    for(int i=0; i<16; i++) {
      bits[i] = 1;
      writeData();
      // pause before next value:
      delay(25);
    }
    for(int i=0; i<16; i++) {
      bits[i] = 0;
      writeData();
      // pause before next value:
      delay(25);
    }  
}

void outIn() {
  clearBits();
    for(int i=0; i<8; i++) {
      bits[i] = 1;
      bits[15-i] = 1;
      writeData();
      // pause before next value:
      delay(50);
    }
    for(int i=0; i<8; i++) {
      bits[i] = 0;
      bits[15-i] = 0;
      writeData();
      // pause before next value:
      delay(50);
    }
}

void clearBits() {
  for(int i=0; i<16; i++) {
    bits[i] = 0;
  }
}

int bitsToB1() {
  int b = 0;
  for(int i=0; i<8; i++)
    b |= bits[i] << i;

  return b;
}

int bitsToB2() {
  int b = 0;
  for(int i=0; i<8; i++)
    b |= bits[i+8] << i;

  return b;
}

void writeData() {
    int data1 = bitsToB2();
    int data2 = bitsToB1();

    // disable LED update
    digitalWrite(latchPin, LOW);

    // shift out the data (second shift register must be send first)
    shiftOut(dataPin, clockPin, MSBFIRST, data1);  
    shiftOut(dataPin, clockPin, MSBFIRST, data2);  

     // update the LEDs
     digitalWrite(latchPin, HIGH);
}
