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

static void sendHttpResponse(
  WiFiClient& clientWeb,
  const char* status,
  const char* contentType,
  const char* body
) {
  clientWeb.print("HTTP/1.1 ");
  clientWeb.println(status);
  clientWeb.print("Content-Type: ");
  clientWeb.println(contentType);
  clientWeb.println("Access-Control-Allow-Origin: *");
  clientWeb.println("Access-Control-Allow-Methods: GET, OPTIONS");
  clientWeb.println("Access-Control-Allow-Headers: Content-Type");
  clientWeb.println("Connection: close");
  clientWeb.println();
  clientWeb.println(body);
}

static void sendTextResponse(WiFiClient& clientWeb, const char* status, const char* body) {
  sendHttpResponse(clientWeb, status, "text/plain", body);
}

static void sendTelemetryResponse(WiFiClient& clientWeb) {
  char payload[180];

  snprintf(
    payload,
    sizeof(payload),
    "{\"motor_der\":%d,\"motor_izq\":%d,\"servo\":%d,\"distancia\":%d}",
    motor_der,
    motor_izq,
    servo,
    distancia
  );

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

  Serial2.write(command);

  Serial.print("Comando enviado al Arduino normal/Nano: ");
  Serial.println(command);

  sendTextResponse(clientWeb, "200 OK", "OK");
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