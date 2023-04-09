/*
 * (c) Matey Nenov (https://www.thinker-talk.com)
 * 
 * Licensed under Creative Commons: By Attribution 3.0
 * http://creativecommons.org/licenses/by/3.0/
 * 
 */
const int MIN_STEP_TIME = 3000; // step time in microseconds (2000 is minimum for the 28BYJ-48 stepper I use, but it has not enough power...

enum StepperState {MOVE_TO_HOME, MOVE_FROM_HOME, MOVE_TO_HOME_AGAIN, MOVE_NEXT};
 
 class BiPolStepper {
  public:
    int state [4] = {LOW, LOW, LOW, LOW};
    bool isUpdated = false;
    long stepTime = MIN_STEP_TIME;

    long position = 0l;
  
    BiPolStepper(int steps, double ppfr) {
      this->steps = steps;
      this->ppfr = ppfr;

      mcrs = micros();
    }

    void update() {
      switch(this->stepperState) {
        case MOVE_TO_HOME: moveToHome(); break;
        case MOVE_FROM_HOME: moveFromHome(); break;
        case MOVE_TO_HOME_AGAIN: moveToHomeAgain(); break;
        case MOVE_NEXT: moveNext(); break;
      }
    }

    void startHoming() {
      this->stepperState = MOVE_TO_HOME;
      this->stepTime = MIN_STEP_TIME;
    }

    void updateLimitSwitch(bool isReached) {
      if(isReached) {
        switch(this->stepperState) {
          case MOVE_TO_HOME: this->stepperState = MOVE_FROM_HOME; break;
          case MOVE_TO_HOME_AGAIN: homeReached(); break;
          default: break;
        }
      }else {
        switch(this->stepperState) {
          case MOVE_FROM_HOME: this->stepperState = MOVE_TO_HOME_AGAIN; break;
          default: break;
        }
      }
    }

    void homeReached() {
      this->stepperState = MOVE_NEXT;
      this->stepsToMake = 0;
      this->dir = true;
      this->stepTime = MIN_STEP_TIME;
      this->position = 0;
    }

    bool isHoming() {
      return this->stepperState != MOVE_NEXT;
    }

    bool isRunning() {
      return stepsToMake > 0 || isHoming();
    }

    void rotateFull(int rot, bool dir = true) {
      stepsToMake = (long)steps * (long)rot;
      this->dir = dir;
    }

    void setNewPosInMM(double pos) {
      moveInMM(pos - getPosInMM());
    }

    void moveInMM(double mm) {
      stepsToMake = abs((mm * (double)this->steps) / this->ppfr);
      this->dir = (mm>0)?true:false;
    }

    double getPosInMM() {
      return ((double)this->position * this->ppfr) / (double)this->steps;
    }

    void clearUpdated() {
      this->isUpdated = false;
    }

  private:
    StepperState stepperState = MOVE_TO_HOME;
    
    int cStep = 0;

    int steps;
    double ppfr = 0.; // progress per full round in mm
    
    long mcrs;

    long stepsToMake = 0;

    bool dir = true;

    void moveToHome() {
      this->dir = false;
      moveNext();
    }

    void moveFromHome() {
      this->dir = true;
      moveNext();
    }

    void moveToHomeAgain() {
      this->dir = false;
      moveNext();
    }

    void moveNext() {
      if(canRotate())
        rotate();
    }

    void rotate() {
      this->isUpdated = true;
      stepsToMake--;
      mcrs = micros();
      calcNextStep();
      updateStep();
    }

    void calcNextStep() {
      if(dir) {
        cStep = (cStep + 1) % 4;
        this->position++;
      }else {
        if(cStep == 0)
          cStep = 4;
        cStep = (cStep - 1) % 4;
        this->position--;
      }
    }

    void updateStep() {
      for(int i=0; i<4; i++)
        state[i] = i == cStep?HIGH:LOW;
    }

    bool canRotate() {
      return (micros() - mcrs >= stepTime) && isRunning();
    }
};
