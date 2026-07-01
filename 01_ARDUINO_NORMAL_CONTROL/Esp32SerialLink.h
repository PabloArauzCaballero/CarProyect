#pragma once
#include <Arduino.h>

// En esta configuracion fisica el ESP32 esta conectado al Serial hardware del Nano:
//   Nano D0/RX <- ESP32 TX2/GPIO17
//   Nano D1/TX -> ESP32 RX2/GPIO16 mediante divisor de voltaje
extern HardwareSerial& esp32Serial;

void setupEsp32SerialLink();
