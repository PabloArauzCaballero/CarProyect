#include "Constants.h"
#include "Globals.h"
#include "NetworkManager.h"
#include "MqttManager.h"
#include "TelemetryManager.h"
#include "WebServerManager.h"

void setup() {
  Serial.begin(SERIAL_MONITOR_BAUD);
  delay(1000);

  Serial.println();
  Serial.println("ESP32 CONTROLADOR DE COMUNICACION");

  setupTelemetrySerial();
  setupWiFi();
  setupMQTT();
  setupWebServer();

  Serial.println("ESP32 listo como controlador de comunicacion.");
}

void loop() {
  connectMQTTNonBlocking();
  mqttLoop();
  readTelemetryFromArduino();
  handleWebClient();
  publishTelemetryEveryInterval();
}
