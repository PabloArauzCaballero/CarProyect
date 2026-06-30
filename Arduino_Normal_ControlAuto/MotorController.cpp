#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "MotorController.h"

static int clampPwm(int speedValue) {
  int pwm = abs(speedValue);
  if (pwm > 255) pwm = 255;
  return pwm;
}

static void setOneMotor(int inA, int inB, int enablePin, int speedValue) {
  int pwm = clampPwm(speedValue);

  if (speedValue > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (speedValue < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  analogWrite(enablePin, pwm);
}

static void setLeftSide(int speedValue) {
  setOneMotor(MOTI1_A, MOTI1_B, VELI1, speedValue);
  setOneMotor(MOTI2_A, MOTI2_B, VELI2, speedValue);
  motor_izq = speedValue;
}

static void setRightSide(int speedValue) {
  setOneMotor(MOTD1_A, MOTD1_B, VELD1, speedValue);
  setOneMotor(MOTD2_A, MOTD2_B, VELD2, speedValue);
  motor_der = speedValue;
}

void setupMotors() {
  pinMode(MOTI1_A, OUTPUT);
  pinMode(MOTI1_B, OUTPUT);
  pinMode(VELI1, OUTPUT);

  pinMode(MOTD1_A, OUTPUT);
  pinMode(MOTD1_B, OUTPUT);
  pinMode(VELD1, OUTPUT);

  pinMode(MOTI2_A, OUTPUT);
  pinMode(MOTI2_B, OUTPUT);
  pinMode(VELI2, OUTPUT);

  pinMode(MOTD2_A, OUTPUT);
  pinMode(MOTD2_B, OUTPUT);
  pinMode(VELD2, OUTPUT);

  stopMotors();
}

void moveForward() {
  setRightSide(DEFAULT_MOTOR_SPEED);
  setLeftSide(DEFAULT_MOTOR_SPEED);
}

void moveBackward() {
  setRightSide(-DEFAULT_MOTOR_SPEED);
  setLeftSide(-DEFAULT_MOTOR_SPEED);
}

void turnLeft() {
  setRightSide(TURN_MOTOR_SPEED);
  setLeftSide(-TURN_MOTOR_SPEED);
}

void turnRight() {
  setRightSide(-TURN_MOTOR_SPEED);
  setLeftSide(TURN_MOTOR_SPEED);
}

void stopMotors() {
  setRightSide(0);
  setLeftSide(0);
}
