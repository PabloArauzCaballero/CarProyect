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

  // SoftwareSerial del Nano en D13/D10 ahora va a 9600.
  // Repetimos el comando 2 veces por seguridad; el Nano filtra duplicados rápidos.
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
