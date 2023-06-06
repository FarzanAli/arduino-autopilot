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
float gyroAngleX, gyroAngleY;
float gyrX, gyrY;
float roll, pitch;

float elapsedTime, time, timePrev;
float totalAngle;

//Neutral servo positionings
int neutralElev = 120;

MPU6050 mpu;

void setup() {
  Serial.begin(9600);
  pinMode(9, INPUT);
  pinMode(13, OUTPUT);
  Wire.begin();
  mpu.initialize();
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
  elapsedTime = (time - timePrev)/1000;

  pulse = pulseIn(9, HIGH) / 1000.0;
  if (pulse > 1.8) {
    digitalWrite(13, HIGH);
    autopilotOn();

    Wire.beginTransmission(0x68);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 6, true);

    accX = Wire.read()<<8|Wire.read();
    accY = Wire.read()<<8|Wire.read();
    accZ = Wire.read()<<8|Wire.read();
    accX = (float)accX/16384.0;
    accY = (float)accY/16384.0;
    accZ = (float)accZ/16384.0 + 0.18;

    // Serial.print(accX);
    // Serial.print(" ");
    // Serial.print(accY);
    // Serial.print(" ");
    // Serial.println(accZ);

    rollAngle = atan(accY/sqrt(accX*accX + accZ*accZ))*1/(3.142/180);
    pitchAngle = -atan(accX/sqrt(accY*accY + accZ*accZ))*1/(3.142/180);
    // Serial.println(rollAngle);

    Wire.beginTransmission(0x68);
    Wire.write(0x43);
    Wire.endTransmission(false);
    Wire.requestFrom(0x68, 4, true);

    gyroAngleX = (Wire.read()<<8|Wire.read())/131.0;
    gyroAngleY = (Wire.read()<<8|Wire.read())/131.0;
    
    gyrX = gyrX + gyroAngleX*elapsedTime;
    gyrY = gyrY + gyroAngleY*elapsedTime;
    roll = 0.96 * gyrX + 0.04 * rollAngle;
    pitch = 0.96 * gyrY + 0.04 * rollAngle;

    Serial.print(pitch);
    Serial.print(" ");
    Serial.println(roll);

    delay(50);
  } else {
    digitalWrite(13, LOW);
    autopilotOff();
  }
}