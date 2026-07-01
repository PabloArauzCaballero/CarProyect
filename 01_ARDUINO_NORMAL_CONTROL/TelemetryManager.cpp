#include <Arduino.h>
#include <string.h>

#include "Constants.h"
#include "Globals.h"
#include "Esp32SerialLink.h"
#include "TelemetryManager.h"

void sendTelemetry() {
  // Protocolo extendido:
  // <motor_der,motor_izq,servo,distancia,izq_del,der_del,izq_tras,der_tras>\n
  char payload[80];
  snprintf(
    payload,
    sizeof(payload),
    "<%d,%d,%d,%d,%d,%d,%d,%d>\n",
    motor_der,
    motor_izq,
    servo_angle,
    distancia_cm,
    motor_izq_del,
    motor_der_del,
    motor_izq_tras,
    motor_der_tras
  );

  esp32Serial.write((const uint8_t*)payload, strlen(payload));
}

void sendTelemetryEveryInterval() {
  unsigned long now = millis();
  if (now - lastTelemetrySent < TELEMETRY_INTERVAL_MS) return;

  lastTelemetrySent = now;
  sendTelemetry();
}
