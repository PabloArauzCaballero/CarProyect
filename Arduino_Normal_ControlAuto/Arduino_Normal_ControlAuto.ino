#include "Constants.h"
#include "Globals.h"
#include "MotorController.h"
#include "ServoController.h"
#include "DistanceSensor.h"
#include "TelemetryManager.h"

// Este sketch usa los pines fisicos RX/TX del Arduino.
// Antes de subirlo, desconecta RX/TX del ESP32.
// Despues de subirlo, vuelve a conectar:
// ESP32 GPIO17/TX2 -> RX Arduino
// ESP32 GPIO16/RX2 <- TX Arduino
// GND <-> GND

void setup() {
  Serial.begin(ESP32_SERIAL_BAUD);

  setupMotors();
  setupServo();
  setupDistanceSensor();
  stopMotors();
}

void loop() {
  readCommandFromESP32();
  updateDistance();
  sendTelemetryEveryInterval();
}

void readCommandFromESP32() {
  while (Serial.available() > 0) {
    char command = Serial.read();

    if (command == '\n' || command == '\r' || command == ' ') {
      continue;
    }

    switch (command) {
      case 'w':
        moveForward();
        break;

      case 's':
        moveBackward();
        break;

      case 'a':
        turnLeft();
        break;

      case 'd':
        turnRight();
        break;

      case 'x':
        stopMotors();
        break;

      case 'q':
        moveServoLeft();
        break;

      case 'e':
        moveServoRight();
        break;

      default:
        break;
    }
  }
}