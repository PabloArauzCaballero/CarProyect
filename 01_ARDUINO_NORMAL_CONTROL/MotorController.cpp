#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "MotorController.h"

static int clampSignedPwm(int speedValue) {
  if (speedValue > 255) return 255;
  if (speedValue < -255) return -255;
  return speedValue;
}

static int clampPwm(int speedValue) {
  int pwm = abs(speedValue);
  if (pwm > 255) pwm = 255;
  return pwm;
}

static void setOneMotorChannel(int inA, int inB, int enablePin, int speedValue) {
  speedValue = clampSignedPwm(speedValue);
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

static void updateSideTelemetry() {
  motor_izq = (motor_izq_del + motor_izq_tras) / 2;
  motor_der = (motor_der_del + motor_der_tras) / 2;
}

void setSideSpeeds(int leftSpeed, int rightSpeed) {
  setWheelSpeeds(leftSpeed, rightSpeed, leftSpeed, rightSpeed);
}

void setWheelSpeeds(int leftFront, int rightFront, int leftRear, int rightRear) {
  leftFront = clampSignedPwm(leftFront);
  rightFront = clampSignedPwm(rightFront);
  leftRear = clampSignedPwm(leftRear);
  rightRear = clampSignedPwm(rightRear);

  // Ahora si son 4 canales reales: cada rueda tiene direccion y PWM propios.
  setOneMotorChannel(MOTI1_A, MOTI1_B, VELI1, leftFront);   // Del. izquierda
  setOneMotorChannel(MOTD1_A, MOTD1_B, VELD1, rightFront);  // Del. derecha
  setOneMotorChannel(MOTI2_A, MOTI2_B, VELI2, leftRear);    // Tras. izquierda
  setOneMotorChannel(MOTD2_A, MOTD2_B, VELD2, rightRear);   // Tras. derecha

  motor_izq_del = leftFront;
  motor_der_del = rightFront;
  motor_izq_tras = leftRear;
  motor_der_tras = rightRear;
  updateSideTelemetry();
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
  setWheelSpeeds(DEFAULT_MOTOR_SPEED, DEFAULT_MOTOR_SPEED, DEFAULT_MOTOR_SPEED, DEFAULT_MOTOR_SPEED);
}

void moveBackward() {
  setWheelSpeeds(-DEFAULT_MOTOR_SPEED, -DEFAULT_MOTOR_SPEED, -DEFAULT_MOTOR_SPEED, -DEFAULT_MOTOR_SPEED);
}

void turnLeft() {
  setWheelSpeeds(-TURN_MOTOR_SPEED, TURN_MOTOR_SPEED, -TURN_MOTOR_SPEED, TURN_MOTOR_SPEED);
}

void turnRight() {
  setWheelSpeeds(TURN_MOTOR_SPEED, -TURN_MOTOR_SPEED, TURN_MOTOR_SPEED, -TURN_MOTOR_SPEED);
}

void stopMotors() {
  setWheelSpeeds(0, 0, 0, 0);
}
