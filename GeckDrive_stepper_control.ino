//GeckoDrive stepper motor control
//GeckoDrive G201X microstepper drive

//Control pins GeckoDrive
// 1 Power GND
// 2 Power (+)
// 3 Motor Phase A
// 4 Motor Phase /A
// 5 Motor Phase B
// 6 Motor Phase /D
// 7 Disable/standby
// 8 Directon
// 9 Step signal
// 10 Common GND Direction and Step
// 11 Current set not in use
// 12 Current set & GND for Disable/Standby

//Arduino pulse generator by TimerOne (PaulStoffregen)
#include <TimerOne.h>
//For Arduino Uno, pins 9 / 10

//Control of speed, number of steps and start/stop by serial commands

//Remco Pieters
//Wageningen University
//v0 - 20160707

//Parameter setup

const int pulsePin = 9;
const int standbyPin = 2;
const int directionPin = 3;
unsigned long period = 500; //Timer period in microseconds
unsigned long steps = 0;
String inData;
volatile unsigned long pulseCount = 0;
unsigned long copypulseCount = 0;
int pulseMode = false;

const int debug = true;


void setup() {
  //Set up serial connection
  Serial.begin(9600);
  Serial.flush();
  //Initialize timer
  Timer1.initialize(period);
  Timer1.pwm(pulsePin, 512); //pulse width 50% duty cycle
  Timer1.stop();
  //Set up digital pins
  pinMode(standbyPin, OUTPUT);
  pinMode(directionPin, OUTPUT);
}

void pulseMotor(void)
{
  pulseCount = pulseCount + 1; //increase when motor makes a step
}

void loop() {
  if(pulseMode == true){
      noInterrupts();
      copypulseCount = pulseCount;
      interrupts();
      Serial.println(copypulseCount);
      if(copypulseCount>steps){
      Timer1.stop();
      Timer1.detachInterrupt();
      pulseMode = false;
     }
    }

  
  //Recieve serial commands
  if (Serial.available() > 0) {
    char recieved = Serial.read();
    inData += recieved;
    // Process message when new line character is recieved
    if (recieved == '\n') {
      //Debug recieved command
      if (debug == true) {
        Serial.print("Received cmd: ");
        Serial.println(inData);
      }

      //Process recieved command
      //Rotation direction
      if (inData.startsWith("dir")) {
        if (inData.substring(4) == "cw\n") {
          //set direction pin high
          digitalWrite(directionPin, HIGH);
          if (debug == true) {
            Serial.println("Direction CW");
          }
        }
        if (inData.substring(4) == "ccw\n") {
          //set direction pin low
          digitalWrite(directionPin, LOW);
          if (debug == true) {
            Serial.println("Direction CCW");
          }
        }
        //Rotation speed, period of pulse train
      }
      if (inData.startsWith("speed")) {
        //set period of timer in microseconds
        String temp_period = inData.substring(6, inData.length() - 1);
        period = temp_period.toInt();
        Timer1.setPeriod(period);
        Timer1.setPwmDuty(pulsePin,512);
        Timer1.stop(); //motor stops after speed change
        if (debug == true) {
          Serial.print("Speed : ");
          Serial.println(period);
        }
      }
      //Motor steps
      if (inData.startsWith("steps")) {
        String temp_steps = inData.substring(6, inData.length() - 1);
        steps = temp_steps.toInt();
        if (debug == true) {
          Serial.print("Steps : ");
          Serial.println(steps);
        }
        //Start with pulseCount at zero
        pulseCount = 0;
        pulseMode = true;
        Timer1.attachInterrupt(pulseMotor);
        Timer1.start();

      }
      //Start/stop motor
      if (inData.startsWith("motor")) {
        if (inData.substring(6) == "start\n") {
          //Start timer
          Timer1.start();
          if (debug == true) {
            Serial.println("START");
          }
        }
        if (inData.substring(6) == "stop\n") {
          //Start timer
          Timer1.stop();
          if (debug == true) {
            Serial.println("STOP");
          }
        }
      }
      inData = ""; // Clear recieved buffer
 

  }
  }

}



