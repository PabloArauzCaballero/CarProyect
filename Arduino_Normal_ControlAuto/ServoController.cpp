#include <Arduino.h>
#include <Servo.h>
#include "Constants.h"
#include "Globals.h"
#include "ServoController.h"

static Servo directionServo;

static void writeServoSafe(int angle) {
  if (angle < SERVO_MIN_ANGLE) angle = SERVO_MIN_ANGLE;
  if (angle > SERVO_MAX_ANGLE) angle = SERVO_MAX_ANGLE;
  servo_angle = angle;
  directionServo.write(servo_angle);
}

void setupServo() {
  directionServo.attach(SERVO_PIN);
  centerServo();
}

void moveServoLeft() {
  writeServoSafe(servo_angle - SERVO_STEP);
}

void moveServoRight() {
  writeServoSafe(servo_angle + SERVO_STEP);
}

void centerServo() {
  writeServoSafe(SERVO_CENTER_ANGLE);
}
