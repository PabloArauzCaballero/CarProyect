#pragma once

static const char WIFI_SSID[]     = "Ciscob2";
static const char WIFI_PASSWORD[] = "Cisco495245@$%";

// IP de la computadora donde corre Mosquitto, NO la IP del ESP32.
static const char MQTT_BROKER_IP[] = "192.168.100.84";
static constexpr int MQTT_PORT = 1883;

static constexpr int GRUPO = 1;
static constexpr unsigned long SERIAL_MONITOR_BAUD = 115200;

// Nuevo cableado indicado:
// ESP32 GPIO16/RX2 <- Nano D10/TX SoftwareSerial
// ESP32 GPIO17/TX2 -> Nano D13/RX SoftwareSerial
static constexpr unsigned long ARDUINO_SERIAL_BAUD = 9600;
static constexpr int ESP32_RX2_PIN = 16;
static constexpr int ESP32_TX2_PIN = 17;
static constexpr int SERIAL2_TIMEOUT_MS = 120;

static constexpr int WEB_SERVER_PORT = 80;
static constexpr unsigned long MQTT_RECONNECT_INTERVAL_MS = 3000;
static constexpr unsigned long TELEMETRY_PUBLISH_INTERVAL_MS = 1000;

static const char CONTROL_TOKEN[] = "";
static const char VALID_COMMANDS[] = "wasdxqe";
