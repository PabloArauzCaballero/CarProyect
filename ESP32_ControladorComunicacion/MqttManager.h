#pragma once
void setupMQTT();
void connectMQTTNonBlocking();
void mqttLoop();
void publishTelemetry();
void publishTelemetryEveryInterval();
