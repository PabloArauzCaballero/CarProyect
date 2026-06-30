#include <Arduino.h>
#include "Constants.h"
#include "Globals.h"
#include "MqttManager.h"

void setupMQTT() {
  mqttClient.setServer(MQTT_BROKER_IP, MQTT_PORT);
}

void connectMQTTNonBlocking() {
  if (mqttClient.connected()) return;

  unsigned long now = millis();
  if (now - lastMqttReconnect < MQTT_RECONNECT_INTERVAL_MS) return;
  lastMqttReconnect = now;

  Serial.print("Intentando conectar a MQTT... ");

  String clientId = "ESP32Client-Grupo" + String(GRUPO);

  if (mqttClient.connect(clientId.c_str())) {
    Serial.println("conectado.");
  } else {
    Serial.print("fallo. Estado MQTT: ");
    Serial.println(mqttClient.state());
  }
}

void mqttLoop() {
  if (mqttClient.connected()) mqttClient.loop();
}

void publishTelemetry() {
  if (!mqttClient.connected()) return;

  char topic[32];
  char payload[160];

  snprintf(topic, sizeof(topic), "robot/%d/sensores", GRUPO);
  snprintf(
    payload,
    sizeof(payload),
    "{\"motor_der\":%d,\"motor_izq\":%d,\"servo\":%d,\"distancia\":%d}",
    motor_der,
    motor_izq,
    servo,
    distancia
  );

  mqttClient.publish(topic, payload);
}

void publishTelemetryEveryInterval() {
  unsigned long now = millis();

  if (now - lastMsg >= TELEMETRY_PUBLISH_INTERVAL_MS) {
    lastMsg = now;
    publishTelemetry();
  }
}
