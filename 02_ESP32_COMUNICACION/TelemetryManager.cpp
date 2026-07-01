#include <Arduino.h>
#include <stdio.h>

#include "Constants.h"
#include "Globals.h"
#include "TelemetryManager.h"

// Parser tolerante para Nano D0/D1 con Serial hardware.
// Acepta:
//   <motor_der,motor_izq,servo,distancia>
//   <motor_der,motor_izq,servo,distancia,izq_del,der_del,izq_tras,der_tras>
//   motor_der,motor_izq,servo,distancia

static String telemetryBuffer;
static bool framedPacketStarted = false;
static bool plainCsvStarted = false;
static unsigned long lastNoiseLogAt = 0;
static unsigned long lastStatusLogAt = 0;

static bool isAllowedCsvChar(char c) {
  return isDigit(c) || c == ',' || c == '-' || c == ' ';
}

static void rememberSerialByte(char c) {
  // Guarda una muestra visible de los últimos bytes recibidos para diagnóstico vía /telemetry.
  if (c == '\n') lastSerialSample += "\\n";
  else if (c == '\r') lastSerialSample += "\\r";
  else if (c == '<') lastSerialSample += "<";
  else if (c == '>') lastSerialSample += ">";
  else if (isPrintable(c)) lastSerialSample += c;
  else {
    char hexText[8];
    snprintf(hexText, sizeof(hexText), "[%02X]", (uint8_t)c);
    lastSerialSample += hexText;
  }

  if (lastSerialSample.length() > 180) {
    lastSerialSample.remove(0, lastSerialSample.length() - 180);
  }
}

static bool parseTelemetryLine(const String& rawLine) {
  String line = rawLine;
  line.trim();

  if (line.length() == 0) return false;

  // Si accidentalmente llega con envoltorio, lo retiramos.
  if (line.startsWith("<")) line.remove(0, 1);
  if (line.endsWith(">")) line.remove(line.length() - 1, 1);
  line.trim();

  int parsedRight = 0;
  int parsedLeft = 0;
  int parsedServo = 90;
  int parsedDistance = 0;
  int parsedLeftFront = 0;
  int parsedRightFront = 0;
  int parsedLeftRear = 0;
  int parsedRightRear = 0;

  int count = sscanf(
    line.c_str(),
    "%d,%d,%d,%d,%d,%d,%d,%d",
    &parsedRight,
    &parsedLeft,
    &parsedServo,
    &parsedDistance,
    &parsedLeftFront,
    &parsedRightFront,
    &parsedLeftRear,
    &parsedRightRear
  );

  if (count != 4 && count != 8) {
    telemetryParseErrorCount++;
    Serial.print("Telemetria ignorada, formato no reconocido: [");
    Serial.print(line);
    Serial.println("]");
    return false;
  }

  motor_der = parsedRight;
  motor_izq = parsedLeft;
  servo = parsedServo;
  distancia = parsedDistance;

  if (count == 8) {
    motor_izq_del = parsedLeftFront;
    motor_der_del = parsedRightFront;
    motor_izq_tras = parsedLeftRear;
    motor_der_tras = parsedRightRear;
  } else {
    motor_izq_del = motor_izq;
    motor_der_del = motor_der;
    motor_izq_tras = motor_izq;
    motor_der_tras = motor_der;
  }

  telemetryOk = true;
  lastTelemetryRaw = line;
  lastTelemetryReceivedAt = millis();

  Serial.print("Telemetria OK: ");
  Serial.println(line);

  return true;
}

static void resetPacketState() {
  telemetryBuffer = "";
  framedPacketStarted = false;
  plainCsvStarted = false;
}

void setupTelemetrySerial() {
  Serial2.begin(ARDUINO_SERIAL_BAUD, SERIAL_8N1, ESP32_RX2_PIN, ESP32_TX2_PIN);
  Serial2.setTimeout(SERIAL2_TIMEOUT_MS);

  Serial.println("Serial2 activo.");
  Serial.println("GPIO16 RX2 <- Nano D1/TX hardware mediante divisor");
  Serial.println("GPIO17 TX2 -> Nano D0/RX hardware");
  Serial.println("Baud Nano <-> ESP32: 9600");
  Serial.println("Protocolos aceptados: CSV de 4 campos y CSV extendido de 8 campos.");
}

void readTelemetryFromArduino() {
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    rememberSerialByte(c);

    // Protocolo con inicio '<' y cierre '>'.
    if (c == '<') {
      framedPacketStarted = true;
      plainCsvStarted = false;
      telemetryBuffer = "";
      continue;
    }

    if (framedPacketStarted) {
      if (c == '>') {
        parseTelemetryLine(telemetryBuffer);
        resetPacketState();
        continue;
      }

      if (c == '\n' || c == '\r') {
        telemetryParseErrorCount++;
        resetPacketState();
        continue;
      }

      if (isAllowedCsvChar(c)) {
        if (telemetryBuffer.length() < 80) {
          telemetryBuffer += c;
        } else {
          telemetryParseErrorCount++;
          Serial.println("Paquete de telemetria descartado por exceso de longitud.");
          resetPacketState();
        }
      } else {
        telemetryParseErrorCount++;
        resetPacketState();
      }
      continue;
    }

    // Protocolo viejo/compatible: CSV plano terminado en \n.
    if (!plainCsvStarted) {
      if (isDigit(c) || c == '-') {
        plainCsvStarted = true;
        telemetryBuffer = "";
        telemetryBuffer += c;
      } else {
        telemetryIgnoredByteCount++;
        if (millis() - lastNoiseLogAt > 3000) {
          lastNoiseLogAt = millis();
          Serial.print("Bytes ignorados antes de paquete valido: ");
          Serial.println(telemetryIgnoredByteCount);
          Serial.print("Muestra serial reciente: ");
          Serial.println(lastSerialSample);
        }
      }
      continue;
    }

    if (plainCsvStarted) {
      if (c == '\n' || c == '\r') {
        if (telemetryBuffer.indexOf(',') >= 0) {
          parseTelemetryLine(telemetryBuffer);
        }
        resetPacketState();
        continue;
      }

      if (isAllowedCsvChar(c)) {
        if (telemetryBuffer.length() < 80) {
          telemetryBuffer += c;
        } else {
          telemetryParseErrorCount++;
          Serial.println("CSV plano descartado por exceso de longitud.");
          resetPacketState();
        }
      } else {
        telemetryParseErrorCount++;
        resetPacketState();
      }
    }
  }

  if (millis() - lastStatusLogAt > 5000) {
    lastStatusLogAt = millis();
    if (!telemetryOk) {
      Serial.print("Aun sin telemetria valida. Muestra serial reciente: ");
      Serial.println(lastSerialSample);
    }
  }
}
