#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "TelemetryManager.h"

void sendTelemetry() {
  // El ESP32 espera SOLO CSV:
  // motor_der,motor_izq,servo,distancia
  Serial.print(motor_der);
  Serial.print(",");
  Serial.print(motor_izq);
  Serial.print(",");
  Serial.print(servo_angle);
  Serial.print(",");
  Serial.println(distancia_cm);
}

void sendTelemetryEveryInterval() {
  unsigned long now = millis();
  if (now - lastTelemetrySent < TELEMETRY_INTERVAL_MS) return;

  lastTelemetrySent = now;
  sendTelemetry();
}
