#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Constants.h"
#include "Esp32SerialLink.h"

// Nano D13 recibe desde ESP32 TX2.
// Nano D10 transmite hacia ESP32 RX2.
SoftwareSerial esp32Serial(NANO_RX_FROM_ESP32_PIN, NANO_TX_TO_ESP32_PIN);

void setupEsp32SerialLink() {
  esp32Serial.begin(ESP32_SERIAL_BAUD);
  esp32Serial.listen();
}
