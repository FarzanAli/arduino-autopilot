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
int elevatorPin = 10;
float pulse;
float accX, accY, accZ;
float rollAngle, pitchAngle;
float gyroRollRate, gyroPitchRate, gyroAngleZ;
float gyrX, gyrY;
float roll, pitch;
float desiredPitch = -8;
float outputPitch = 0;
float gyroRoll, gyroPitch;

float error, previous_error_pitch, PID;
float P = 0;
float I = 0;
float D = 0;
double kp_pitch = 3.55;
double ki_pitch = 0.003;
double kd_pitch = 2.05;
float elev_max = 50;

float elapsedTime, time, timePrev;
float totalAngle;

//Neutral servo positionings
int neutralElev = 90;

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
  // Moving the servo
  // elevator.write(30);
}

void autopilotOff() {
  pinMode(elevatorPin, LOW);
  elevator.detach();
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
    // Serial.print(gyroRoll);
    // Serial.print(" ");
    // Serial.println(gyroPitch);

    //PITCH
    error = gyroPitch - desiredPitch;

    P = kp_pitch * error;

    if (-3 < error < 3) {
      I += (ki_pitch * error);
    }

    D = kd_pitch * ((error - previous_error_pitch) / elapsedTime);

    PID = P + I + D;

    if (PID > elev_max) {
      PID = elev_max;
    }
    if (PID < -elev_max) {
      PID = -elev_max;
    }
    outputPitch = neutralElev - (PID);
    previous_error_pitch = error;
    elevator.write(outputPitch);

  } else {
    digitalWrite(13, LOW);
    autopilotOff();
  }
}