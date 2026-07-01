#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "ServoController.h"

static unsigned long lastServoPulseAtUs = 0;
static unsigned int servoPulseWidthUs = 1500;

static int clampServoAngle(int angle) {
  if (angle < SERVO_MIN_ANGLE) return SERVO_MIN_ANGLE;
  if (angle > SERVO_MAX_ANGLE) return SERVO_MAX_ANGLE;
  return angle;
}

static unsigned int angleToPulseUs(int angle) {
  angle = clampServoAngle(angle);
  long pulse = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  if (pulse < SERVO_MIN_US) pulse = SERVO_MIN_US;
  if (pulse > SERVO_MAX_US) pulse = SERVO_MAX_US;
  return (unsigned int)pulse;
}

void serviceServo() {
  unsigned long nowUs = micros();
  if (nowUs - lastServoPulseAtUs < SERVO_PERIOD_US) return;

  lastServoPulseAtUs = nowUs;
  digitalWrite(PIN_SERVO, HIGH);
  delayMicroseconds(servoPulseWidthUs);
  digitalWrite(PIN_SERVO, LOW);
}

void delayKeepingServo(unsigned long durationMs) {
  unsigned long startedAt = millis();
  while (millis() - startedAt < durationMs) {
    serviceServo();
    delay(2);
  }
}

void setServoAngle(int angle) {
  servo_angle = clampServoAngle(angle);
  servo = servo_angle;
  servoPulseWidthUs = angleToPulseUs(servo_angle);

  // Envia unos pulsos inmediatos para que el movimiento empiece aunque el loop este ocupado.
  for (int i = 0; i < 3; i++) {
    serviceServo();
    delayKeepingServo(20);
  }
}

void setupServo() {
  pinMode(PIN_SERVO, OUTPUT);
  digitalWrite(PIN_SERVO, LOW);
  centerServo();
}

void moveServoLeft() {
  setServoAngle(servo_angle - SERVO_STEP);
}

void moveServoRight() {
  setServoAngle(servo_angle + SERVO_STEP);
}

void centerServo() {
  setServoAngle(SERVO_CENTER_ANGLE);
}
