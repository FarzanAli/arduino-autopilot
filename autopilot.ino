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

//Neutral servo positionings
int neutralElev = 120;

void setup() {
  Serial.begin(9600);
  pinMode(9, INPUT);
  pinMode(13, OUTPUT);
  Wire.begin();
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
    accZ = (float)accZ/16384.0;

    Serial.print(accX);
    Serial.print(" ");
    Serial.print(accY);
    Serial.print(" ");
    Serial.println(accZ);
    delay(500);
  } else {
    digitalWrite(13, LOW);
    autopilotOff();
  }
}