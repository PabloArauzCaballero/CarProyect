#include <Arduino.h>
#include <WiFi.h>
#include "Constants.h"
#include "NetworkManager.h"

void setupWiFi() {
  Serial.println();
  Serial.print("Conectando a WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi conectado.");
  Serial.print("IP DEL ESP32: ");
  Serial.println(WiFi.localIP());
}
