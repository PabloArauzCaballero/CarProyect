#include "Constants.h"
#include "Globals.h"
#include "Esp32SerialLink.h"
#include "MotorController.h"
#include "ServoController.h"
#include "DistanceSensor.h"
#include "TelemetryManager.h"

void readCommandFromESP32();
void executeCommand(char command);

// Esta version respeta tu NUEVO cableado:
// - ESP32 RX2  <- Nano D10  (TX SoftwareSerial del Nano)
// - ESP32 TX2  -> Nano D13  (RX SoftwareSerial del Nano)
// - GND        <-> GND
//
// NO borra tus variables globales: memoriaAcciones, memoriaTiempos,
// pasoActual, modoReproduccion, pines originales y variables de ajuste siguen presentes.

static void rememberAction(char action) {
  if (pasoActual >= MAX_PASOS) return;

  memoriaAcciones[pasoActual] = action;
  memoriaTiempos[pasoActual] = millis();
  pasoActual++;
}

void executeCommand(char command) {
  if (command != 'x') {
    last_command = command;
  }

  obstacle_stop_active = false;

  switch (command) {
    case 'w':
      if (distancia_cm > 0 && distancia_cm <= STOP_DISTANCE_CM) {
        stopMotors();
        obstacle_stop_active = true;
        last_command = 'x';
      } else {
        moveForward();
        rememberAction('F');
      }
      break;

    case 's':
      moveBackward();
      rememberAction('B');
      break;

    case 'a':
      turnLeft();
      rememberAction('I');
      break;

    case 'd':
      turnRight();
      rememberAction('D');
      break;

    case 'x':
      stopMotors();
      last_command = 'x';
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

void setup() {
  // D0/D1 quedan libres para USB/Monitor Serial si necesitas debug local.
  // La comunicacion real con ESP32 va por SoftwareSerial en D13/D10.
  Serial.begin(115200);
  setupEsp32SerialLink();

  setupMotors();
  setupServo();
  setupDistanceSensor();
  stopMotors();
}

void loop() {
  readCommandFromESP32();
  updateDistance();

  if (last_command == 'w' && distancia_cm > 0 && distancia_cm <= STOP_DISTANCE_CM) {
    stopMotors();
    obstacle_stop_active = true;
    last_command = 'x';
  }

  sendTelemetryEveryInterval();
}

void readCommandFromESP32() {
  esp32Serial.listen();

  static char lastProcessedCommand = 0;
  static unsigned long lastProcessedAt = 0;

  while (esp32Serial.available() > 0) {
    char command = esp32Serial.read();

    if (command == '\n' || command == '\r' || command == ' ') {
      continue;
    }

    if (!(command == 'w' || command == 'a' || command == 's' || command == 'd' ||
          command == 'x' || command == 'q' || command == 'e')) {
      continue;
    }

    unsigned long now = millis();

    // El ESP32 repite comandos para compensar alguna perdida aislada.
    // A 9600 esto es estable; este filtro evita duplicar q/e o movimientos.
    if (command == lastProcessedCommand && now - lastProcessedAt < 90) {
      continue;
    }

    lastProcessedCommand = command;
    lastProcessedAt = now;
    executeCommand(command);
  }
}
