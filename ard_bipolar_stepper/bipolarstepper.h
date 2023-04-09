/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */
 const int STEP_TIME = 3; // step time in ms (2 is minimum for the 28BYJ-48 stepper I used
 
 class BiPolStepper {
  public:
    BiPolStepper(int pin_1, int pin_2, int pin_3, int pin_4, int steps) {
      pins[0] = pin_1;
      pins[1] = pin_2;
      pins[2] = pin_3;
      pins[3] = pin_4;

      this->steps = steps;

      initPins();
      
      mls = millis();
    }

    void update() {
      if(canRotate())
        rotate();
    }

    bool isRunning() {
      return stepsToMake > 0;
    }

    void rotateFull(int rot, bool dir = true) {
      stepsToMake = steps * rot;
      this->dir = dir;
    }

  private:
    int pins[4];
    int state [4] = {LOW, LOW, LOW, LOW};
    int cStep = 0;

    int steps;
    
    long mls;

    long stepsToMake = 0;

    bool dir = true;

    void initPins() {
      for(int i=0; i<4; i++)
        pinMode(pins[i], OUTPUT);
    }

    void rotate() {
      stepsToMake--;
      mls = millis();
      calcNextStep();
      updateStep();
      setStep();
    }

    void calcNextStep() {
      if(dir) {
        cStep = (cStep + 1) % 4;  
      }else {
        if(cStep == 0)
          cStep = 4;
        cStep = (cStep - 1) % 4;
      }
    }

    void updateStep() {
      for(int i=0; i<4; i++)
        state[i] = i == cStep?HIGH:LOW;
    }

    void setStep() {
      for(int i=0; i<4; i++)
        digitalWrite(pins[i], state[i]);
    }

    bool canRotate() {
      return (millis() - mls >= STEP_TIME) && isRunning();
    }
};
