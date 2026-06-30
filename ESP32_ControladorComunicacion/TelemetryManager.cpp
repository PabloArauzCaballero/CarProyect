#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "TelemetryManager.h"

void setupTelemetrySerial() {
  Serial2.begin(ARDUINO_SERIAL_BAUD, SERIAL_8N1, ESP32_RX2_PIN, ESP32_TX2_PIN);
  Serial2.setTimeout(SERIAL2_TIMEOUT_MS);

  Serial.println("Serial2 activo.");
  Serial.println("GPIO16 RX2 <- TX del Arduino");
  Serial.println("GPIO17 TX2 -> RX del Arduino");
}

void readTelemetryFromArduino() {
  if (Serial2.available() <= 0) return;

  String line = Serial2.readStringUntil('\n');
  line.trim();

  if (line.length() == 0) return;

  int index1 = line.indexOf(',');
  int index2 = line.indexOf(',', index1 + 1);
  int index3 = line.indexOf(',', index2 + 1);

  if (index1 == -1 || index2 == -1 || index3 == -1) {
    Serial.print("Telemetria ignorada, no es CSV: [");
    Serial.print(line);
    Serial.println("]");
    return;
  }

  motor_der = line.substring(0, index1).toInt();
  motor_izq = line.substring(index1 + 1, index2).toInt();
  servo     = line.substring(index2 + 1, index3).toInt();
  distancia = line.substring(index3 + 1).toInt();

  Serial.print("Telemetria OK: ");
  Serial.println(line);
}
