#include <Arduino.h>

#include "Constants.h"
#include "Globals.h"
#include "TelemetryManager.h"

static String telemetryBuffer;

static bool parseTelemetryLine(const String& rawLine) {
  String line = rawLine;
  line.trim();

  if (line.length() == 0) return false;

  int index1 = line.indexOf(',');
  int index2 = line.indexOf(',', index1 + 1);
  int index3 = line.indexOf(',', index2 + 1);

  if (index1 == -1 || index2 == -1 || index3 == -1) {
    Serial.print("Telemetria ignorada, no es CSV completo: [");
    Serial.print(line);
    Serial.println("]");
    return false;
  }

  motor_der = line.substring(0, index1).toInt();
  motor_izq = line.substring(index1 + 1, index2).toInt();
  servo = line.substring(index2 + 1, index3).toInt();
  distancia = line.substring(index3 + 1).toInt();

  Serial.print("Telemetria OK: ");
  Serial.println(line);

  return true;
}

void setupTelemetrySerial() {
  Serial2.begin(ARDUINO_SERIAL_BAUD, SERIAL_8N1, ESP32_RX2_PIN, ESP32_TX2_PIN);
  Serial2.setTimeout(SERIAL2_TIMEOUT_MS);

  Serial.println("Serial2 activo.");
  Serial.println("GPIO16 RX2 <- TX del Arduino");
  Serial.println("GPIO17 TX2 -> RX del Arduino");
}

void readTelemetryFromArduino() {
  while (Serial2.available() > 0) {
    char c = Serial2.read();

    if (c == '\r') {
      continue;
    }

    if (c == '\n') {
      parseTelemetryLine(telemetryBuffer);
      telemetryBuffer = "";
      continue;
    }

    if (telemetryBuffer.length() < 80) {
      telemetryBuffer += c;
    } else {
      Serial.println("Buffer de telemetria limpiado por exceso de longitud.");
      telemetryBuffer = "";
    }
  }
}