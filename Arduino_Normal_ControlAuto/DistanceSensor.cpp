#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "DistanceSensor.h"

void setupDistanceSensor() {
  pinMode(ULTRASONIC_TRIG_PIN, OUTPUT);
  pinMode(ULTRASONIC_ECHO_PIN, INPUT);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
}

int measureDistanceCm() {
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRASONIC_TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRASONIC_TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ULTRASONIC_ECHO_PIN, HIGH, ULTRASONIC_TIMEOUT_US);
  if (duration == 0) return distancia_cm;

  int cm = duration * 0.034 / 2;
  if (cm <= 0) return distancia_cm;

  return cm;
}

void updateDistance() {
  unsigned long now = millis();
  if (now - lastDistanceUpdate < DISTANCE_UPDATE_INTERVAL_MS) return;

  lastDistanceUpdate = now;
  distancia_cm = measureDistanceCm();
}
