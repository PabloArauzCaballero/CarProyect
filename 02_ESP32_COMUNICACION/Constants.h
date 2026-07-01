#pragma once

static const char WIFI_SSID[]     = "iPhone del baisa";
static const char WIFI_PASSWORD[] = "khaledvela";

// IP de la computadora donde corre Mosquitto, NO la IP del ESP32.
static const char MQTT_BROKER_IP[] = "172.20.10.3";
static constexpr int MQTT_PORT = 1883;

static constexpr int GRUPO = 1;
static constexpr unsigned long SERIAL_MONITOR_BAUD = 115200;

// Comunicacion ESP32 <-> Arduino Nano segun la configuracion fisica final:
// ESP32 GPIO17/TX2 -> Nano D0/RX hardware
// ESP32 GPIO16/RX2 <- Nano D1/TX hardware mediante divisor de voltaje
static constexpr unsigned long ARDUINO_SERIAL_BAUD = 9600;
static constexpr int ESP32_RX2_PIN = 16;
static constexpr int ESP32_TX2_PIN = 17;
static constexpr int SERIAL2_TIMEOUT_MS = 120;

static constexpr int WEB_SERVER_PORT = 80;
static constexpr unsigned long MQTT_RECONNECT_INTERVAL_MS = 3000;
static constexpr unsigned long TELEMETRY_PUBLISH_INTERVAL_MS = 1000;

static const char CONTROL_TOKEN[] = "";
static const char VALID_COMMANDS[] = "wasdxqeczt";

static constexpr int MANUAL_PWM_MIN = -255;
static constexpr int MANUAL_PWM_MAX = 255;
static constexpr int SERVO_MIN_ANGLE = 30;
static constexpr int SERVO_MAX_ANGLE = 150;
