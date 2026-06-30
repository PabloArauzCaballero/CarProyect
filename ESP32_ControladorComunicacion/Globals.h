#pragma once
#include <WiFi.h>
#include <PubSubClient.h>

extern int motor_der;
extern int motor_izq;
extern int servo;
extern int distancia;
extern unsigned long lastMsg;
extern unsigned long lastMqttReconnect;
extern WiFiClient espClient;
extern PubSubClient mqttClient;
extern WiFiServer webServer;
