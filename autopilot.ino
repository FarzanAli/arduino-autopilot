/*
* Autopilot
* author: Farzan Ali Faisal
* date: 28/05/2023
*/

#include "Wire.h"
#include "I2Cdev.h"
#include "MPU6050.h"
#include "Servo.h"

Servo elevator;
Servo aileron;

int elevatorPin = 10;
int aileronPin = 11;

float pulse;

float accX, accY, accZ;
float rollAngle, pitchAngle;

float gyroRollRate, gyroPitchRate, gyroRoll, gyroPitch;

float desiredPitch = -8;
float desiredRoll = -8;
float outputPitch = 0;
float outputRoll = 0;

float error, prevPitchError, prevRollError, PID;
float P = 0;
float I = 0;
float D = 0;

double pRoll=2;//3.55
double iRoll=0.002;//0.003
double dRoll=1;//2.05

double pPitch = 3.55;
double iPitch = 0.003;
double dPitch = 2.05;

float elapsedTime, time, timePrev;

//Neutral servo positionings
int neutralElev = 90;
int neutralAil = 90;

//Max servo positionings
float maxElev = 50;
float maxAil = 50;

MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  pinMode(9, INPUT);
  pinMode(13, OUTPUT);
  Wire.begin();
  mpu.initialize();
  delay(1000);
  time = millis();
}

void autopilotOn() {
  elevator.attach(elevatorPin);
  aileron.attach(aileronPin);
}

void autopilotOff() {
  pinMode(elevatorPin, LOW);
  pinMode(aileronPin, LOW);
  elevator.detach();
  aileron.detach();
}

void loop() {
  timePrev = time;
  time = millis();
  elapsedTime = (time - timePrev) / 1000;

  pulse = pulseIn(9, HIGH) / 1000.0;
  if (pulse > 1.8) {
    digitalWrite(13, HIGH);
    autopilotOn();

    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);

    accX = Wire.read() << 8 | Wire.read();
    accY = Wire.read() << 8 | Wire.read();
    accZ = Wire.read() << 8 | Wire.read();
    accX = ((float)accX / 16384.0);
    accY = ((float)accY / 16384.0);
    accZ = (float)(accZ / 16384.0) + 0.18;

    Serial.print(accX);
    Serial.print(" ");
    Serial.print(accY);
    Serial.print(" ");
    Serial.println(accZ);

    rollAngle = atan((accY) / sqrt((accX * accX) + (accZ * accZ))) * (1 / (3.142 / 180));
    pitchAngle = atan(-1 * (accX) / sqrt((accY * accY) + (accZ * accZ))) * (1 / (3.142 / 180));
    // Serial.println(pitchAngle);
    // Serial.println(rollAngle);

    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 4, true);

    gyroRollRate = (Wire.read() << 8 | Wire.read()) / 131.0;
    gyroPitchRate = (Wire.read() << 8 | Wire.read()) / 131.0;
    // gyroAngleZ = (Wire.read() << 8 | Wire.read()) / 131.0;

    gyroRollRate = gyroRollRate * elapsedTime;
    gyroPitchRate = gyroPitchRate * elapsedTime;

    gyroRoll = 0.96 * (gyroRoll + gyroRollRate) + 0.04 * rollAngle;
    gyroPitch = 0.96 * (gyroPitch + gyroPitchRate) + 0.04 * pitchAngle;

    //Pitch
    error = gyroPitch - desiredPitch;

    P = pPitch * error;

    if (-3 < error < 3) {
      I += (iPitch * error);
    }

    D = dPitch * ((error - prevPitchError) / elapsedTime);

    PID = P + I + D;

    if (PID > maxElev) {
      PID = maxElev;
    }
    if (PID < -maxElev) {
      PID = -maxElev;
    }
    outputPitch = neutralElev - PID;
    prevPitchError = error;
    elevator.write(outputPitch);

    //Roll
    error = gyroRoll - desiredRoll;

    P = pRoll * error;

    if(-3 < error < 3){
      I += (iRoll * error);
    }

    D = dRoll * ((error - prevRollError) / elapsedTime);

    PID = P + I + D;

    if(PID > maxAil){
      PID = maxAil;
    }
    if(PID < maxAil * -1){
      PID = maxAil * -1;
    }

    outputRoll = neutralAil + PID;
    prevRollError = error;
    aileron.write(outputRoll);

  } else {
    digitalWrite(13, LOW);
    autopilotOff();
  }
}