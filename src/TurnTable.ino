// Turn Table Controller
// Ver 1.0 2023.10.23 K.Ohe First Release
// Ver 1.1 2023.11.05 K.Ohe Support Kawazu PCB
#include <Stepper.h>
#include <DabbleESP32.h>
#define KAWAZU
// for kwazu version
#ifdef KAWAZU
    #define IN1  1
    #define IN2  3
    #define IN3  5
    #define IN4  7
#else
// for ooe version
    #define IN1  9
    #define IN2  7
    #define IN3  5
    #define IN4  3
#endif

#define GRAPH_HEIGHT  240
#define GRAPH_WIDTH   320
#define DISP_TIME      10
#define FWD  1
#define REW -1
#define WAIT_TIME     300
#ifdef KAWAZU
    #define DEVICE_NAME   "TANTE2"  // for kawazu version
#else
    #define DEVICE_NAME   "TANTE" // for ooe version
#endif
#define INIT_SPEED      6
// globals
const int16_t NBSTEPS = 2048;   // for 28BYJ48  1/64 x 32 Steps
int16_t dispTime   = DISP_TIME;
int16_t degree     = 0;
int16_t oldAngle   = 0;
int16_t clockWise  = FWD;
int16_t motorSpeed = INIT_SPEED;
bool    stopFlag   = false;

// instances
Stepper myStepper(NBSTEPS, IN1, IN3, IN2, IN4);

//========================================
// dispSpeed : Display Speed Value
//========================================
void dispSpeed(int16_t speed)
{
    char buf[64];

    sprintf(buf, "Speed =%4d   ", speed);
    Serial.println(buf);
}

//========================================
// dispDegree : Display Degree Value
//========================================
void dispDegree(int16_t degree)
{
    char buf[64];
    sprintf(buf, "%04d", degree);
    Serial.println(buf);
}

//========================================
// btnCheck : Check Buttons of ESP32-S3-Box 
//========================================
void btnCheck()
{
    Dabble.processInput(); 

    if (GamePad.isSelectPressed()) {
        Serial.print("STOP");
        stopFlag = true;
        delay(WAIT_TIME);
    }
    if (GamePad.isStartPressed()){
        Serial.println("START");
        stopFlag = false;
        delay(WAIT_TIME);
    }
    if (GamePad.isSquarePressed()) {
        Serial.println("Left");
        clockWise = REW;
        delay(WAIT_TIME);
    }
    if (GamePad.isCirclePressed()) {
        Serial.println("Right");
        clockWise = FWD;
        delay(WAIT_TIME);
    }
    if (GamePad.isTrianglePressed()) {
        Serial.println("Triangle");
        motorSpeed++;
        if (motorSpeed > 15) motorSpeed = 15;
        myStepper.setSpeed(motorSpeed);
        if (motorSpeed > 1) {
            stopFlag = false;
        }
        delay(WAIT_TIME);
    }
    if (GamePad.isCrossPressed()) {
        Serial.println("Cross");
        motorSpeed--;
        if (motorSpeed < 1) {
            motorSpeed = INIT_SPEED;
            stopFlag = true;
        }
        myStepper.setSpeed(motorSpeed);
        delay(WAIT_TIME);
    }
    if (stopFlag) {
        int angle = GamePad.getAngle();
        if (angle <= 0) {
            myStepper.setSpeed(motorSpeed);
            clockWise = FWD;
        } else {
            angle = (360 - (angle - 90)) % 360;
            if (angle > 180) angle -= 360;
            if (angle != 0) {
                if (angle > 0) {
                    clockWise = FWD;
                } else {
                    clockWise = REW;
                }
                int wSpeed = abs(angle / 18);
                if (wSpeed < 1) wSpeed = 1;
                myStepper.setSpeed(wSpeed);
                myStepper.step(clockWise);

                Serial.print("Angle: ");
                Serial.println(angle);
            }
        }
    }
}

//========================================
// setup : Arduino setup
//========================================
void setup(void)
{
    Serial.begin(115200);
    Dabble.begin(DEVICE_NAME);
    Serial.println("Starting...");

    // set the motor speed:
    myStepper.setSpeed(abs(motorSpeed));
    dispSpeed(motorSpeed);
    dispDegree(0);
}

//========================================
// loop : Arduino Main Loop
//========================================
int stepCount = 0;         // number of steps the motor has taken
void loop()
{
    btnCheck();
    if (!stopFlag) {
        myStepper.step(clockWise);
        if (dispTime >= DISP_TIME) {
            degree = (abs)((int)((stepCount) / 5.69) % 360);
            //dispDegree(degree);
            dispTime = 0;
        } else {
            dispTime++;
        }
        //Serial.print("steps:");
        //Serial.println(stepCount);
        stepCount += clockWise;
    } else {
        delay(1);
    }
}