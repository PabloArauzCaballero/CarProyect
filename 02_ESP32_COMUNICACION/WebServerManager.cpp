#include <Arduino.h>
#include <string.h>

#include "Constants.h"
#include "Globals.h"
#include "WebServerManager.h"

static bool isCommandValid(char command) {
  for (int i = 0; VALID_COMMANDS[i] != '\0'; i++) {
    if (VALID_COMMANDS[i] == command) return true;
  }

  return false;
}

static bool tokenIsRequired() {
  return strlen(CONTROL_TOKEN) > 0;
}

static bool requestHasValidToken(const String& requestLine) {
  if (!tokenIsRequired()) return true;

  String expected = "token=" + String(CONTROL_TOKEN);
  return requestLine.indexOf(expected) >= 0;
}

static int clampInt(int value, int minValue, int maxValue) {
  if (value < minValue) return minValue;
  if (value > maxValue) return maxValue;
  return value;
}

static String escapeJson(const String& value) {
  String escaped = "";

  for (unsigned int i = 0; i < value.length(); i++) {
    char c = value.charAt(i);

    if (c == '\\') escaped += "\\\\";
    else if (c == '"') escaped += "\\\"";
    else if (c == '\n') escaped += "\\n";
    else if (c == '\r') escaped += "\\r";
    else escaped += c;
  }

  return escaped;
}

static String getQueryParam(const String& requestLine, const String& key) {
  String search = key + "=";
  int start = requestLine.indexOf(search);
  if (start < 0) return "";

  start += search.length();
  int endAmp = requestLine.indexOf('&', start);
  int endSpace = requestLine.indexOf(' ', start);
  int end = -1;

  if (endAmp < 0) end = endSpace;
  else if (endSpace < 0) end = endAmp;
  else end = min(endAmp, endSpace);

  if (end < 0) end = requestLine.length();
  return requestLine.substring(start, end);
}

static bool hasQueryParam(const String& requestLine, const String& key) {
  return requestLine.indexOf(key + "=") >= 0;
}

static bool getQueryInt(const String& requestLine, const String& key, int& outValue) {
  String raw = getQueryParam(requestLine, key);
  raw.trim();
  if (raw.length() == 0) return false;

  int start = 0;
  if (raw.charAt(0) == '-') {
    if (raw.length() == 1) return false;
    start = 1;
  }

  for (int i = start; i < raw.length(); i++) {
    if (!isDigit(raw.charAt(i))) return false;
  }

  outValue = raw.toInt();
  return true;
}

static void sendHttpResponse(
  WiFiClient& clientWeb,
  const char* status,
  const char* contentType,
  const String& body
) {
  clientWeb.print("HTTP/1.1 ");
  clientWeb.println(status);
  clientWeb.print("Content-Type: ");
  clientWeb.println(contentType);
  clientWeb.println("Access-Control-Allow-Origin: *");
  clientWeb.println("Access-Control-Allow-Methods: GET, OPTIONS");
  clientWeb.println("Access-Control-Allow-Headers: Content-Type");
  clientWeb.println("Cache-Control: no-store");
  clientWeb.println("Connection: close");
  clientWeb.println();
  clientWeb.println(body);
}

static void sendTextResponse(WiFiClient& clientWeb, const char* status, const String& body) {
  sendHttpResponse(clientWeb, status, "text/plain", body);
}

static void sendTelemetryResponse(WiFiClient& clientWeb) {
  long ageMs = telemetryOk ? (long)(millis() - lastTelemetryReceivedAt) : -1;

  String payload = "{";
  payload += "\"motor_der\":" + String(motor_der) + ",";
  payload += "\"motor_izq\":" + String(motor_izq) + ",";
  payload += "\"motor_izq_del\":" + String(motor_izq_del) + ",";
  payload += "\"motor_der_del\":" + String(motor_der_del) + ",";
  payload += "\"motor_izq_tras\":" + String(motor_izq_tras) + ",";
  payload += "\"motor_der_tras\":" + String(motor_der_tras) + ",";
  payload += "\"servo\":" + String(servo) + ",";
  payload += "\"distancia\":" + String(distancia) + ",";
  payload += "\"telemetry_ok\":" + String(telemetryOk ? "true" : "false") + ",";
  payload += "\"age_ms\":" + String(ageMs) + ",";
  payload += "\"parse_errors\":" + String(telemetryParseErrorCount) + ",";
  payload += "\"ignored_bytes\":" + String(telemetryIgnoredByteCount) + ",";
  payload += "\"serial_sample\":\"" + escapeJson(lastSerialSample) + "\",";
  payload += "\"last_raw\":\"" + escapeJson(lastTelemetryRaw) + "\"";
  payload += "}";

  sendHttpResponse(clientWeb, "200 OK", "application/json", payload);
}

static void sendFrameToArduino(const String& frame) {
  Serial2.print(frame);
  Serial2.flush();
}

static bool processManualRequest(const String& line, WiFiClient& clientWeb) {
  if (line.indexOf("GET /manual?") < 0 && line.indexOf("GET /control?manual=1") < 0) {
    return false;
  }

  if (!requestHasValidToken(line)) {
    Serial.println("Manual rechazado: token invalido o ausente.");
    sendTextResponse(clientWeb, "401 Unauthorized", "TOKEN_INVALIDO");
    return true;
  }

  int leftFront = 0;
  int rightFront = 0;
  int leftRear = 0;
  int rightRear = 0;
  int leftSide = 0;
  int rightSide = 0;
  int angle = servo;

  bool hasLf = getQueryInt(line, "lf", leftFront) || getQueryInt(line, "fl", leftFront);
  bool hasRf = getQueryInt(line, "rf", rightFront) || getQueryInt(line, "fr", rightFront);
  bool hasLr = getQueryInt(line, "lr", leftRear) || getQueryInt(line, "rl", leftRear);
  bool hasRr = getQueryInt(line, "rr", rightRear);
  bool hasLeft = getQueryInt(line, "left", leftSide) || getQueryInt(line, "l", leftSide);
  bool hasRight = getQueryInt(line, "right", rightSide) || getQueryInt(line, "r", rightSide);
  bool hasServo = getQueryInt(line, "servo", angle) || getQueryInt(line, "angle", angle);

  if (hasLf) leftFront = clampInt(leftFront, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasRf) rightFront = clampInt(rightFront, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasLr) leftRear = clampInt(leftRear, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasRr) rightRear = clampInt(rightRear, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasLeft) leftSide = clampInt(leftSide, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasRight) rightSide = clampInt(rightSide, MANUAL_PWM_MIN, MANUAL_PWM_MAX);
  if (hasServo) angle = clampInt(angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE);

  if (hasLeft && hasRight && hasServo) {
    String frame = "<C," + String(leftSide) + "," + String(rightSide) + "," + String(angle) + ">";
    sendFrameToArduino(frame);
    Serial.print("Manual por lado + servo enviado: ");
    Serial.println(frame);
    sendTextResponse(clientWeb, "200 OK", "OK:" + frame);
    return true;
  }

  if (hasLeft && hasRight) {
    String frame = "<S," + String(leftSide) + "," + String(rightSide) + ">";
    sendFrameToArduino(frame);
    Serial.print("Manual por lado enviado: ");
    Serial.println(frame);
    sendTextResponse(clientWeb, "200 OK", "OK:" + frame);
    return true;
  }

  if (hasLf && hasRf && hasLr && hasRr && hasServo) {
    String frame = "<B," + String(leftFront) + "," + String(rightFront) + "," + String(leftRear) + "," + String(rightRear) + "," + String(angle) + ">";
    sendFrameToArduino(frame);
    Serial.print("Manual ruedas + servo enviado: ");
    Serial.println(frame);
    sendTextResponse(clientWeb, "200 OK", "OK:" + frame);
    return true;
  }

  if (hasLf && hasRf && hasLr && hasRr) {
    String frame = "<M," + String(leftFront) + "," + String(rightFront) + "," + String(leftRear) + "," + String(rightRear) + ">";
    sendFrameToArduino(frame);
    Serial.print("Manual ruedas enviado: ");
    Serial.println(frame);
    sendTextResponse(clientWeb, "200 OK", "OK:" + frame);
    return true;
  }

  if (hasServo) {
    String frame = "<A," + String(angle) + ">";
    sendFrameToArduino(frame);
    Serial.print("Angulo servo enviado: ");
    Serial.println(frame);
    sendTextResponse(clientWeb, "200 OK", "OK:" + frame);
    return true;
  }

  sendTextResponse(clientWeb, "400 Bad Request", "MANUAL_INVALIDO: usa lf/rf/lr/rr, left/right y/o servo");
  return true;
}

static void processRequestLine(const String& line, WiFiClient& clientWeb, bool& requestProcessed) {
  if (line.startsWith("OPTIONS ")) {
    requestProcessed = true;
    sendTextResponse(clientWeb, "204 No Content", "");
    return;
  }

  if (line.indexOf("GET /telemetry") >= 0) {
    requestProcessed = true;

    if (!requestHasValidToken(line)) {
      Serial.println("Telemetria rechazada: token invalido o ausente.");
      sendTextResponse(clientWeb, "401 Unauthorized", "TOKEN_INVALIDO");
      return;
    }

    sendTelemetryResponse(clientWeb);
    return;
  }

  if (processManualRequest(line, clientWeb)) {
    requestProcessed = true;
    return;
  }

  if (line.indexOf("GET /control?cmd=") < 0) {
    return;
  }

  requestProcessed = true;

  if (!requestHasValidToken(line)) {
    Serial.println("Comando rechazado: token invalido o ausente.");
    sendTextResponse(clientWeb, "401 Unauthorized", "TOKEN_INVALIDO");
    return;
  }

  int pos = line.indexOf("cmd=");
  char command = line.charAt(pos + 4);

  if (!isCommandValid(command)) {
    Serial.print("Comando invalido rechazado: ");
    Serial.println(command);
    sendTextResponse(clientWeb, "400 Bad Request", "COMANDO_INVALIDO");
    return;
  }

  // Comando simple compatible con el control existente.
  for (int i = 0; i < 2; i++) {
    Serial2.write(command);
    Serial2.flush();
    delay(5);
  }

  Serial.print("Comando enviado al Arduino normal/Nano: ");
  Serial.println(command);

  sendTextResponse(clientWeb, "200 OK", "OK:" + String(command));
}

void setupWebServer() {
  webServer.begin();
  Serial.println("Servidor HTTP nativo activo.");
}

void handleWebClient() {
  WiFiClient clientWeb = webServer.available();

  if (!clientWeb) return;

  String currentLine = "";
  bool responseSent = false;
  bool requestProcessed = false;
  unsigned long startedAt = millis();

  while (clientWeb.connected() && millis() - startedAt < 1000) {
    if (!clientWeb.available()) {
      delay(1);
      continue;
    }

    char c = clientWeb.read();

    if (c == '\n') {
      if (currentLine.length() == 0) {
        if (!responseSent && !requestProcessed) {
          sendTextResponse(clientWeb, "200 OK", "OK");
          responseSent = true;
        }

        break;
      }

      if (!responseSent) {
        processRequestLine(currentLine, clientWeb, requestProcessed);

        if (requestProcessed) {
          responseSent = true;
        }
      }

      currentLine = "";
    } else if (c != '\r') {
      currentLine += c;
    }
  }

  clientWeb.stop();
}
