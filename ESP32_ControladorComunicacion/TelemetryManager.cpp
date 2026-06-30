#include <Arduino.h>

#include "Constants.h"
#include "Globals.h"
#include "TelemetryManager.h"

// Parser tolerante para tu caso real D10/D13 con SoftwareSerial en el Nano.
// Acepta estos dos formatos:
//   <motor_der,motor_izq,servo,distancia>\n   // protocolo nuevo
//   motor_der,motor_izq,servo,distancia\n     // protocolo viejo, por si el Nano no quedó actualizado

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

static bool isSignedNumber(const String& value) {
  if (value.length() == 0) return false;

  int start = 0;
  if (value.charAt(0) == '-') {
    if (value.length() == 1) return false;
    start = 1;
  }

  for (int i = start; i < value.length(); i++) {
    if (!isDigit(value.charAt(i))) return false;
  }

  return true;
}

static bool parseTelemetryLine(const String& rawLine) {
  String line = rawLine;
  line.trim();

  if (line.length() == 0) return false;

  // Si accidentalmente llega con envoltorio, lo retiramos.
  if (line.startsWith("<")) line.remove(0, 1);
  if (line.endsWith(">")) line.remove(line.length() - 1, 1);
  line.trim();

  int index1 = line.indexOf(',');
  int index2 = line.indexOf(',', index1 + 1);
  int index3 = line.indexOf(',', index2 + 1);

  if (index1 == -1 || index2 == -1 || index3 == -1) {
    telemetryParseErrorCount++;
    Serial.print("Telemetria ignorada, no es CSV completo: [");
    Serial.print(line);
    Serial.println("]");
    return false;
  }

  String rightMotorText = line.substring(0, index1);
  String leftMotorText = line.substring(index1 + 1, index2);
  String servoText = line.substring(index2 + 1, index3);
  String distanceText = line.substring(index3 + 1);

  rightMotorText.trim();
  leftMotorText.trim();
  servoText.trim();
  distanceText.trim();

  if (!isSignedNumber(rightMotorText) || !isSignedNumber(leftMotorText) ||
      !isSignedNumber(servoText) || !isSignedNumber(distanceText)) {
    telemetryParseErrorCount++;
    Serial.print("Telemetria ignorada, contiene valores no numericos: [");
    Serial.print(line);
    Serial.println("]");
    return false;
  }

  motor_der = rightMotorText.toInt();
  motor_izq = leftMotorText.toInt();
  servo = servoText.toInt();
  distancia = distanceText.toInt();

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
  Serial.println("GPIO16 RX2 <- Nano D10/TX SoftwareSerial");
  Serial.println("GPIO17 TX2 -> Nano D13/RX SoftwareSerial");
  Serial.println("Baud Nano <-> ESP32: 9600");
  Serial.println("Protocolos aceptados: <motor_der,motor_izq,servo,distancia> y motor_der,motor_izq,servo,distancia");
}

void readTelemetryFromArduino() {
  while (Serial2.available() > 0) {
    char c = Serial2.read();
    rememberSerialByte(c);

    // Protocolo nuevo con inicio '<' y cierre '>'.
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
        // Algunos paquetes pueden llegar con salto antes del cierre si se cortan.
        telemetryParseErrorCount++;
        resetPacketState();
        continue;
      }

      if (isAllowedCsvChar(c)) {
        if (telemetryBuffer.length() < 50) {
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
        if (telemetryBuffer.length() < 50) {
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
