#include <array>

const double R = 91.; // work area radius
const double C = 135.; // arm length
const double D = 24.; // tool radius
const double MAX_R = 58.;

// in mm
struct Pos {
  double x;
  double y;
  double z;
  double a1;
  double a2;
  double a3;
};

enum RobotState {HOMING, PAINTING, CATCH_THE_BALL};

class DeltaRobot {
  public:
    BiPolStepper *s1, *s2, *s3;

    double zOffs = 28.;
 
    DeltaRobot(BiPolStepper * s1, BiPolStepper * s2, BiPolStepper * s3) {
      Serial.println("Initializing Delta Robot...");
      this->s1 = s1;
      this->s2 = s2;
      this->s3 = s3;

      this->a0 = sqrt(sq(C) - sq(R - D));

      initToolVectors();

      initPoints();
//      initCircle();
      initSpirale();
      Serial.println("Delta Robot Initialized!");
      initiateHoming();
     }

     void initPoints() {
        for(int i=0; i<this->points.size(); i++) {
          this->points[i] = {0., 0., 0., 0., 0., 0.};
        }
     }

     void initCircle() {
        this->stepCount = 42;
        initPos(0, 0., 0., this->zOffs);
        initPos(1, 10., 0., this->zOffs);
        initPos(2, 20., 0., this->zOffs);
        initPos(3, 30., 0., this->zOffs);
        initPos(4, 40., 0., this->zOffs);
        for(int i=5; i<this->stepCount; i++) {
          double rad = (i-5) * 10. * 0.0174532925;
          initPos(i, MAX_R*cos(rad), MAX_R*sin(rad), this->zOffs);
        }
     }

     void initSpirale() {
        this->stepCount = 366;
        initPos(0, 0., 0., this->zOffs);
        initPos(1, 10., 0., this->zOffs);
        initPos(2, 20., 0., this->zOffs);
        initPos(3, 30., 0., this->zOffs);
        initPos(4, 40., 0., this->zOffs);

        for(int j=0; j<10; j++) {
          double r = MAX_R * (1. - 0.1 * j);
          for(int i=0; i<36; i++) {
            double rad = i * 10. * 0.0174532925;
            initPos(5+j*36+i, r*cos(rad), r*sin(rad), this->zOffs);
          }
        }
        initPos(365, 0., 0., this->zOffs);
     }

    void updateRobot() {
      switch(this->state) {
        case HOMING: updateHoming(); break;
        case PAINTING: updatePainting(); break;
        case CATCH_THE_BALL: updateCatchTheBall(); break;
      }
    }

    void updateLimitSwitches(bool ls1, bool ls2, bool ls3) {
      this->s1->updateLimitSwitch(ls1);
      this->s2->updateLimitSwitch(ls2);
      this->s3->updateLimitSwitch(ls3);
    }

    void initiateHoming() {
      this->s1->startHoming();
      this->s2->startHoming();
      this->s3->startHoming();
      this->state = HOMING;
    }

  private:
    RobotState state = HOMING;
    double a0;
    double dx1, dy1, dx2, dy2, dx3, dy3;
    int stepCount = 0;
    std::array<Pos, 500> points;
    int currStep = 0;

    double csq = sq(C);

    double px1 = 0.;
    double py1 = -R;
    double px2 = sqrt(3./4.*sq(R));
    double py2 = R/2.;
    double px3 = -px2;
    double py3 = py2;

    void initToolVectors() {
      dx1 = 0;
      dy1 = -D;

      dx2 = sqrt(3.*sq(D)/4.);
      dy2 = D/2.;

      dx3 = -dx2;
      dy3 = dy2;
    }

    void startHoming() {
      this->state = HOMING;
      this->s1->startHoming();
      this->s2->startHoming();
      this->s3->startHoming();
    }

    void updateHoming() {
      this->s1->update();
      this->s2->update();
      this->s3->update();

      if(!(this->s1->isHoming() || this->s2->isHoming() || this->s3->isHoming())) {
        this->state = CATCH_THE_BALL;
        this->currStep = 0;
      }
    }

    void updateCatchTheBall() {
      this->state = PAINTING;
    }

    void updatePainting() {
      if(s1->isRunning())
        s1->update();
      if(s2->isRunning())
        s2->update();
      if(s3->isRunning())
        s3->update();

      if(!(s1->isRunning() || s2->isRunning() || s3->isRunning())) {
        setNextStep();
      }
    }

    void setNextStep() {
      this->currStep++;
      if(this->currStep >= this->stepCount) {
        initiateHoming();
        return;
      }
//      this->currStep = (this->currStep + 1) % this->stepCount;

      Pos p = this->points[this->currStep];

      double ds1 = abs(p.a1 - s1->getPosInMM());
      double ds2 = abs(p.a2 - s2->getPosInMM());
      double ds3 = abs(p.a3 - s3->getPosInMM());
      double maxds = (ds1 > ds2)?(ds1 > ds3?ds1:ds3):(ds2 > ds3?ds2:ds3);

      maxds *= 3000.;
      long st1 = isCloseToZero(ds1)?1000:round(maxds / ds1);
      long st2 = isCloseToZero(ds2)?1000:round(maxds / ds2);
      long st3 = isCloseToZero(ds3)?1000:round(maxds / ds3);

      long maxTime = 10000l;
      if(st1 > maxTime)
        st1 = maxTime;
      if(st2 > maxTime)
        st2 = maxTime;
      if(st3 > maxTime)
        st3 = maxTime;

      /*
      sprintf(str, "ds1: %f   st1: %d\nds2: %f   st2: %d\nds3: %f   st3: %d\n", ds1, st1, ds2, st2, ds3, st3);
      Serial.print(str);
      delay(100);
      */
      
      s1->setNewPosInMM(p.a1);
      s1->stepTime = st1;
      s2->setNewPosInMM(p.a2);
      s2->stepTime = st2;
      s3->setNewPosInMM(p.a3);
      s3->stepTime = st3;
    }

    bool isCloseToZero(double v) {
      return abs(v) < 0.01;
    }

    void initPos(int pos, double x, double y, double z) {
      /*
      Serial.print("Setting point: ");
      char str [100];
      sprintf(str, "%d:  %f, %f, %f \n", pt, p.x, p.y, p.z);
      Serial.print(str);
      delay(100);
*/
      double x1 = x + dx1;
      double y1 = y + dy1;
      double x2 = x + dx2;
      double y2 = y + dy2;
      double x3 = x + dx3;
      double y3 = y + dy3;
      
      double b1 = sq(px1 - x1) + sq(py1 - y1);
      double b2 = sq(px2 - x2) + sq(py2 - y2);
      double b3 = sq(px3 - x3) + sq(py3 - y3);

/*
      sprintf(str, "b1: %f \nb2: %f \nb3: %f \n", b1, b2, b3);
      Serial.print(str);
      delay(100);
*/

      double a1 = z + this->a0 - sqrt(csq - b1);
      double a2 = z + this->a0 - sqrt(csq - b2);
      double a3 = z + this->a0 - sqrt(csq - b3);

/*
      sprintf(str, "a1: %f \na2: %f \na3: %f \n", a1, a2, a3);
      Serial.print(str);
      delay(100);
*/

      this->points[pos].x = x;
      this->points[pos].y = y;
      this->points[pos].z = z;
      this->points[pos].a1 = a1;
      this->points[pos].a2 = a2;
      this->points[pos].a3 = a3;
    }
};
