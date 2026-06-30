#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "MotorController.h"

static void setRightMotor(int speedValue) {
  int pwm = abs(speedValue);
  if (pwm > 255) pwm = 255;

  if (speedValue > 0) {
    digitalWrite(MOTOR_RIGHT_IN1, HIGH);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);
  } else if (speedValue < 0) {
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, HIGH);
  } else {
    digitalWrite(MOTOR_RIGHT_IN1, LOW);
    digitalWrite(MOTOR_RIGHT_IN2, LOW);
  }

  analogWrite(MOTOR_RIGHT_EN, pwm);
  motor_der = speedValue;
}

static void setLeftMotor(int speedValue) {
  int pwm = abs(speedValue);
  if (pwm > 255) pwm = 255;

  if (speedValue > 0) {
    digitalWrite(MOTOR_LEFT_IN1, HIGH);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
  } else if (speedValue < 0) {
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, HIGH);
  } else {
    digitalWrite(MOTOR_LEFT_IN1, LOW);
    digitalWrite(MOTOR_LEFT_IN2, LOW);
  }

  analogWrite(MOTOR_LEFT_EN, pwm);
  motor_izq = speedValue;
}

void setupMotors() {
  pinMode(MOTOR_RIGHT_EN, OUTPUT);
  pinMode(MOTOR_RIGHT_IN1, OUTPUT);
  pinMode(MOTOR_RIGHT_IN2, OUTPUT);
  pinMode(MOTOR_LEFT_EN, OUTPUT);
  pinMode(MOTOR_LEFT_IN1, OUTPUT);
  pinMode(MOTOR_LEFT_IN2, OUTPUT);
  stopMotors();
}

void moveForward() {
  setRightMotor(DEFAULT_MOTOR_SPEED);
  setLeftMotor(DEFAULT_MOTOR_SPEED);
}

void moveBackward() {
  setRightMotor(-DEFAULT_MOTOR_SPEED);
  setLeftMotor(-DEFAULT_MOTOR_SPEED);
}

void turnLeft() {
  setRightMotor(DEFAULT_MOTOR_SPEED);
  setLeftMotor(-DEFAULT_MOTOR_SPEED);
}

void turnRight() {
  setRightMotor(-DEFAULT_MOTOR_SPEED);
  setLeftMotor(DEFAULT_MOTOR_SPEED);
}

void stopMotors() {
  setRightMotor(0);
  setLeftMotor(0);
}
