// Turn Table Controller
// Ver 1.0 2023.10.23 K.Ohe First Release
// Ver 1.1 2023.11.05 K.Ohe Support Kawazu PCB
// Ver 1.2 2024.01.15 K.Ohe Support zero position detection
#include <Stepper.h>
#include <DabbleESP32.h>
#define KAWAZU
#define ZEROCAL
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

#ifdef ZEROCAL
    #define ADC_PIN  		 15
    #define ADC_ZERO 	    500
#endif

#define GRAPH_HEIGHT  240
#define GRAPH_WIDTH   320
#define DISP_TIME      10
#define FWD             1
#define REW            -1
#define WAIT_TIME     200
#define LIMIT_DEGREE   45

#ifdef KAWAZU
    #define DEVICE_NAME   "TANTE2"  // for kawazu version
#else
    #define DEVICE_NAME   "TANTE" // for ooe version
#endif
#define INIT_SPEED      6
// globals
int16_t NBSTEPS = 2048;   // for 28BYJ48  1/64 x 32 Steps
int16_t dispTime   = DISP_TIME;
int16_t degree     = 0;
int16_t oldAngle   = 0;
int16_t clockWise  = FWD;
int16_t motorSpeed = INIT_SPEED;
int16_t angle      = 0;
int16_t stepCount  = 0;         // number of steps the motor has taken

bool    stopFlag   = false;

enum {
    FREERUN = 0,
    SHAKERUN,
};
uint16_t runMode    = SHAKERUN;
uint16_t oldRunMode = SHAKERUN;


// instances
Stepper myStepper(NBSTEPS, IN1, IN3, IN2, IN4);

//========================================
// dispSpeed : Display Speed Value
//========================================
void dispSpeed(int16_t speed)
{
    char buf[64];

    sprintf(buf, "Speed = %4d   ", speed);
    Serial.println(buf);
}

//========================================
// dispDegree : Display Degree Value
//========================================
void dispDegree(int16_t degree)
{
    char buf[64];
    sprintf(buf, "degree= %4d", degree);
    Serial.println(buf);
}

//========================================
// btnCheck : Check Buttons of ESP32-S3-Box 
//========================================
void btnCheck()
{
    Dabble.processInput(); 

    if (GamePad.isSelectPressed()) {        // "SELECT RUN MODE"
        if (runMode == FREERUN) {
            runMode = SHAKERUN;
            oldRunMode = FREERUN;
            Serial.println("SHAKERUN");
        } else {
            runMode = FREERUN;
            oldRunMode = SHAKERUN;
            Serial.println("FREERUN");
        }
        delay(WAIT_TIME);
    }
    if (GamePad.isStartPressed()){          // "START / STOP" 
        if (stopFlag == true ) {
            stopFlag = false;
            Serial.println("START");
        } else {
            stopFlag = true;
            Serial.println("STOP");
        }
        delay(WAIT_TIME);
    }
    if (GamePad.isSquarePressed()) {        // "□"
        Serial.println("Left");
        clockWise = REW;
        delay(WAIT_TIME);
    }
    if (GamePad.isCirclePressed()) {        // "◯"
        Serial.println("Right");
        clockWise = FWD;
        delay(WAIT_TIME);
    }
    if (GamePad.isTrianglePressed()) {      // "△"
        Serial.print("Triangle");
        motorSpeed++;
        if (motorSpeed > 15) motorSpeed = 15;
        myStepper.setSpeed(motorSpeed);
        if (motorSpeed > 1) {
            stopFlag = false;
        }
        Serial.printf("\t: SpeedUp %3d\n", motorSpeed);
        delay(WAIT_TIME);
    }
    if (GamePad.isCrossPressed()) {         // "☓"
        Serial.print("Cross");
        motorSpeed--;
        if (motorSpeed < 1) {
            motorSpeed = INIT_SPEED;
            stopFlag = true;
        }
        myStepper.setSpeed(motorSpeed);
        delay(WAIT_TIME);
        Serial.printf("\t: SpeedUp %3d\n", motorSpeed);
    }
    angle = GamePad.getAngle();
}

//========================================
// zeroCalibrate() : calibrate zero position
//========================================
void zeroCalibrate()
{
    uint16_t 	voltage;

    clockWise  = REW;
    for (int i = 0; i < NBSTEPS * 2; i++) {
        myStepper.step(clockWise);
        voltage = analogReadMilliVolts(ADC_PIN);
        //Serial.printf("voltage = %d mV\n", voltage);
        if (voltage >= ADC_ZERO) {
            degree = 0;
            break;
        }
        delay(1);
    }
    clockWise  = FWD;
}

//========================================
// setup : Arduino setup
//========================================
void setup(void)
{
    Serial.begin(115200);
    Serial.println("Initilize...");
   	pinMode(ADC_PIN, ANALOG);				// ADC in
    analogSetAttenuation(ADC_11db);			// ADC ATT Level

   // set the motor speed:
    myStepper.setSpeed(abs(motorSpeed));
    dispSpeed(motorSpeed);
#ifdef ZEROCAL
    zeroCalibrate();
    delay(1000);
#endif
    Dabble.begin(DEVICE_NAME);
    Serial.println("Starting...");
    dispDegree(0);
}

//========================================
// joyMode() : joystick control mode
//========================================
void joyMode()
{
    int16_t wClockWise;
    int16_t wAngle;

    if (angle <= 0) {
        myStepper.setSpeed(motorSpeed);
        clockWise = FWD;
    } else {    
        wAngle = (360 - (angle - 90)) % 360;
        if (wAngle > 180) wAngle -= 360;
        if (wAngle != 0) {
            if (wAngle > 0) {
                wClockWise = FWD;
            } else {
                wClockWise = REW;
            }
            int wSpeed = abs(wAngle / 18);
            if (wSpeed < 1) wSpeed = 1;
            myStepper.setSpeed(wSpeed);
            myStepper.step(wClockWise);
            stepCount += wClockWise;
            Serial.print("wAngle: ");
            Serial.println(wAngle);
        }
    }
}

//========================================
// freeRunMode() : free Running
//========================================
void freeRunMode()
{
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
}

//========================================
// shakeRunMode() : shake head Running
//========================================
void shakeRunMode()
{
    int16_t wSpeed;

    if (oldRunMode != runMode) {
        oldRunMode = runMode;
#ifdef ZEROCAL
        Serial.println("start ZeroCalibration");
        zeroCalibrate();
        delay(1000);
#endif
        degree = 0;
        stepCount = 0;
    }
    degree = (int)((stepCount) / 5.69) % 360;
    //dispDegree(degree);
    wSpeed = motorSpeed;
    if (degree >= 0) {
        if (degree > LIMIT_DEGREE / 3 * 2) {
            wSpeed = map(degree, LIMIT_DEGREE / 3 * 2, LIMIT_DEGREE, motorSpeed, 2);
        }
        if (clockWise == FWD && degree >= LIMIT_DEGREE) {
            clockWise = REW;
        }
    } else {
        if (degree <= -LIMIT_DEGREE / 3 * 2) {
            wSpeed = map(degree, -LIMIT_DEGREE , -LIMIT_DEGREE / 3 * 2, 2, motorSpeed);
        }
        if (clockWise == REW && degree <= -LIMIT_DEGREE) {
            clockWise = FWD;
        }
    }
    myStepper.setSpeed(wSpeed);
    //dispSpeed(wSpeed);
    myStepper.step(clockWise);
    stepCount += clockWise;
}

//========================================
// loop : Arduino Main Loop
//========================================
void loop()
{
    btnCheck();
    if (stopFlag) {
        joyMode();
    } else {
        if (runMode == FREERUN) {
            freeRunMode();
        } else {
            shakeRunMode();
        }
    }
    delay(1);

}