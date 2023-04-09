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
    int state [4] = {LOW, LOW, LOW, LOW};
    bool isUpdated = false;
  
    BiPolStepper(int steps) {
      this->steps = steps;

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
      stepsToMake = (long)steps * (long)rot;
      this->dir = dir;
    }

    void clearUpdated() {
      this->isUpdated = false;
    }

  private:
    int cStep = 0;

    int steps;
    
    long mls;

    long stepsToMake = 0;

    bool dir = true;

    void rotate() {
      this->isUpdated = true;
      stepsToMake--;
      mls = millis();
      calcNextStep();
      updateStep();
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

    bool canRotate() {
      return (millis() - mls >= STEP_TIME) && isRunning();
    }
};
