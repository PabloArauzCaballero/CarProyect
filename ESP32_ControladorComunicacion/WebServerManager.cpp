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

static void sendHttpResponse(WiFiClient& clientWeb, const char* status, const char* body) {
  clientWeb.print("HTTP/1.1 ");
  clientWeb.println(status);
  clientWeb.println("Content-Type: text/plain");
  clientWeb.println("Access-Control-Allow-Origin: *");
  clientWeb.println("Connection: close");
  clientWeb.println();
  clientWeb.println(body);
}

static void processRequestLine(const String& line, WiFiClient& clientWeb, bool& commandProcessed) {
  if (line.indexOf("GET /control?cmd=") < 0) return;

  commandProcessed = true;

  if (!requestHasValidToken(line)) {
    Serial.println("Comando rechazado: token invalido o ausente.");
    sendHttpResponse(clientWeb, "401 Unauthorized", "TOKEN_INVALIDO");
    return;
  }

  int pos = line.indexOf("cmd=");
  char command = line.charAt(pos + 4);

  if (!isCommandValid(command)) {
    Serial.print("Comando invalido rechazado: ");
    Serial.println(command);
    sendHttpResponse(clientWeb, "400 Bad Request", "COMANDO_INVALIDO");
    return;
  }

  Serial2.print(command);
  Serial.print("Comando enviado al Arduino normal/Nano: ");
  Serial.println(command);

  sendHttpResponse(clientWeb, "200 OK", "OK");
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
  bool commandProcessed = false;

  while (clientWeb.connected()) {
    if (!clientWeb.available()) continue;

    char c = clientWeb.read();

    if (c == '\n') {
      if (currentLine.length() == 0) {
        if (!responseSent && !commandProcessed) {
          sendHttpResponse(clientWeb, "200 OK", "OK");
          responseSent = true;
        }
        break;
      }

      if (!responseSent) {
        processRequestLine(currentLine, clientWeb, commandProcessed);
        if (commandProcessed) responseSent = true;
      }

      currentLine = "";
    } else if (c != '\r') {
      currentLine += c;
    }
  }

  clientWeb.stop();
}
