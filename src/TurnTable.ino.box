// Turn Table Controller
// Ver1.0 2023.10.23 K.Ohe
#define LGFX_ESP32_S3_BOX_LITE
#define LGFX_USE_V1
#include <LGFX_AUTODETECT.hpp>  // クラス"LGFX"を準備します
#include <LovyanGFX.hpp>
#include <Stepper.h>

#define IN1 38
#define IN2 39
#define IN3 40
#define IN4 41
#define ADC_PIN  	    1
#define GRAPH_HEIGHT  240
#define GRAPH_WIDTH   320
#define DISP_TIME      10
#define FWD  5
#define REW -5

// globals
const int16_t NBSTEPS = 2048;   // for 28BYJ48  1/64 x 32 Steps
int16_t dispTime   = DISP_TIME;
int16_t degree     = 0;
int16_t clockWise  = REW;
int16_t motorSpeed = -8;
bool    stopFlag   = false;

// instances
Stepper myStepper(NBSTEPS, IN1, IN3, IN2, IN4);
static LGFX lcd;
#ifdef min
#undef min
#endif

//========================================
// dispSpeed : Display Speed Value
//========================================
void dispSpeed(int16_t speed)
{
    char buf[64];
    //lcd.fillRect(5, 172, 140, 32, TFT_BLUE);
    sprintf(buf, "Speed =%4d   ", speed);
    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1.0, 1.0);
    lcd.setTextColor(TFT_RED, TFT_BLUE);
    lcd.setCursor(5, 172);
    lcd.print(buf);
}

//========================================
// dispDegree : Display Degree Value
//========================================
void dispDegree(int16_t degree)
{
    char buf[64];
    //lcd.fillRect(10, 48, 319, 124, TFT_BLUE);
    sprintf(buf, "%04d", degree);
    lcd.setFont(&fonts::Font7);
    lcd.setTextSize(2.4, 2.4);
    lcd.setTextColor(TFT_YELLOW, TFT_BLUE);
    lcd.setCursor(10, 48);
    lcd.print(buf);
}

//========================================
// btnCheck : Check Buttons of ESP32-S3-Box 
//========================================
void btnCheck()
{
    int16_t voltage;

    voltage = analogReadMilliVolts(ADC_PIN);
    //Serial.printf("voltage = %4d\n", voltage);

    if (voltage >= 500 && voltage < 1000) {                 // SW3_ON = 820mV
        //Serial.println("SW3_ON");
        motorSpeed++;
        if (motorSpeed >= 15) motorSpeed = 15;
        if (motorSpeed > 0) {
            clockWise = FWD;
            myStepper.setSpeed(motorSpeed);
        } else if (motorSpeed < 0) {
            clockWise = REW;
            myStepper.setSpeed(abs(motorSpeed));
        }
        dispSpeed(motorSpeed);
        delay(200);
    } else if (voltage >= 1000 && voltage < 2200) {         // SW2_ON = 1980mV
        //Serial.println("SW2_ON");
        //motorSpeed = 1;
        //myStepper.setSpeed(motorSpeed);
        //dispSpeed(motorSpeed);
        stopFlag = !stopFlag;
        if (!stopFlag) {
            lcd.setFont(&fonts::Font4);
            lcd.setTextSize(1.0, 1.0);
            lcd.setTextColor(TFT_BLACK, TFT_WHITE);
            lcd.setCursor(125, 210);
            lcd.print(" STOP ");
        } else {
            lcd.setFont(&fonts::Font4);
            lcd.setTextSize(1.0, 1.0);
            lcd.setTextColor(TFT_BLACK, TFT_WHITE);
            lcd.setCursor(125, 210);
            lcd.print("START ");
        }
        delay(200);
    } else if (voltage >= 2200 && voltage < 2500) {         // SW1_ON = 2410mV
        //Serial.println("SW1_ON");
        motorSpeed--;
        if (motorSpeed <= -15) motorSpeed = -15;
        if (motorSpeed > 0) {
            clockWise = FWD;
            myStepper.setSpeed(motorSpeed);
        } else if (motorSpeed < 0) {
            clockWise = REW;
            myStepper.setSpeed(abs(motorSpeed));
        }
        dispSpeed(motorSpeed);
        delay(200);
    }
}

//========================================
// setup : Arduino setup
//========================================
void setup(void)
{
    lcd.init();
    Serial.begin(115200);

	analogSetAttenuation(ADC_11db);			// ADC ATT Level
	pinMode(ADC_PIN, ANALOG);				// ADC in

    Serial.println("Starting...");

    lcd.println("Starting...");

    lcd.startWrite();
    lcd.fillRect(0, 0, GRAPH_WIDTH, GRAPH_HEIGHT, TFT_BLUE);
    lcd.setCursor(10, 16);
    lcd.setFont(&fonts::Font4);
    lcd.setTextSize(1.0, 1.0);
    lcd.setTextColor(TFT_YELLOW);
    lcd.setCursor(16, 8);
    lcd.print("# Turn Table Controller #");
    lcd.setTextColor(TFT_BLACK, TFT_WHITE);
    lcd.setCursor(15, 210);
    lcd.print(" REW ");
    lcd.setCursor(125, 210);
    lcd.print(" STOP ");
    lcd.setCursor(240, 210);
    lcd.print(" FWD ");
    lcd.setTextColor(TFT_YELLOW);
    lcd.setCursor(250, 172);
    lcd.print("Deg.");
    lcd.setCursor(8, 72);
    lcd.setFont(&fonts::Font7);
    lcd.setTextSize(2.4, 2.4);
    lcd.setTextColor(TFT_YELLOW, TFT_BLUE);

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
            dispDegree(degree);
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