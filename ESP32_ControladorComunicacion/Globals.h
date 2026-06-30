#pragma once
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

extern int motor_der;
extern int motor_izq;
extern int servo;
extern int distancia;
extern unsigned long lastMsg;
extern unsigned long lastMqttReconnect;

extern bool telemetryOk;
extern String lastTelemetryRaw;
extern unsigned long lastTelemetryReceivedAt;
extern unsigned long telemetryParseErrorCount;
extern unsigned long telemetryIgnoredByteCount;
extern String lastSerialSample;
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern WiFiServer webServer;
