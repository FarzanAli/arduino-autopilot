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

MPU6050 mpu;
int16_t ax, ay, az;
int16_t gx, gy, gz;

struct MyData {
  byte X;
  byte Y;
  byte Z;
};

MyData data;

float pulse;
void setup() {
  Serial.begin(9600);
  pinMode(9, INPUT);
  pinMode(13, OUTPUT);
  Wire.begin();
  mpu.initialize();
}

void loop() {
  pulse = pulseIn(9, HIGH) / 1000.0;
  if (pulse > 1.8) {
    digitalWrite(13, HIGH);
    autopilotOn();
    printGyroData();
  } else {
    digitalWrite(13, LOW);
    autopilotOff();
  }
}

void autopilotOn() {
  elevator.attach(elevatorPin);
  // Moving the servo
  elevator.write(30);
}

void autopilotOff() {
  //move servo back to neutral positioning
  elevator.write(90);
  // delay(100);
  pinMode(10, LOW);
  elevator.detach();
}

void printGyroData() {
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  data.X = map(ax, -17000, 17000, 0, 255);  // X axis data
  data.Y = map(ay, -17000, 17000, 0, 255);
  data.Z = map(az, -17000, 17000, 0, 255);  // Y axis data
  delay(100);
  Serial.print("Axis X = ");
  Serial.print(data.X);
  Serial.print("  ");
  Serial.print("Axis Y = ");
  Serial.print(data.Y);
  Serial.print("  ");
  Serial.print("Axis Z  = ");
  Serial.println(data.Z);
}