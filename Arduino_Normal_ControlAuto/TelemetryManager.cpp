#include <Arduino.h>
#include <string.h>

#include "Constants.h"
#include "Globals.h"
#include "Esp32SerialLink.h"
#include "TelemetryManager.h"

void sendTelemetry() {
  // Protocolo robusto para SoftwareSerial:
  // <motor_der,motor_izq,servo,distancia>\n
  // Importante: se envia en una sola escritura para reducir cortes/intercalados.
  char payload[40];
  snprintf(
    payload,
    sizeof(payload),
    "<%d,%d,%d,%d>\n",
    motor_der,
    motor_izq,
    servo_angle,
    distancia_cm
  );

  esp32Serial.write((const uint8_t*)payload, strlen(payload));
}

void sendTelemetryEveryInterval() {
  unsigned long now = millis();
  if (now - lastTelemetrySent < TELEMETRY_INTERVAL_MS) return;

  lastTelemetrySent = now;
  sendTelemetry();
}
