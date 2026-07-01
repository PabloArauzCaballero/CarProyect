#include <Arduino.h>

#include "Constants.h"
#include "Esp32SerialLink.h"

// D0/D1 del Nano son Serial hardware. No usamos SoftwareSerial porque la
// configuracion fisica esta fija en D0/D1.
HardwareSerial& esp32Serial = Serial;

void setupEsp32SerialLink() {
  esp32Serial.begin(ESP32_SERIAL_BAUD);
}
