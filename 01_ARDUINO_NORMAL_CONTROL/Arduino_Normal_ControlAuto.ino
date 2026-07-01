#include "Constants.h"
#include "Globals.h"
#include "Esp32SerialLink.h"
#include "MotorController.h"
#include "ServoController.h"
#include "DistanceSensor.h"
#include "TelemetryManager.h"

void readCommandFromESP32();
void executeCommand(char command);
void resetNanoState();
void runStartupSelfTest();
void processFramedCommand(const char* frame);

static void rememberAction(char action) {
  if (pasoActual >= MAX_PASOS) return;

  memoriaAcciones[pasoActual] = action;
  memoriaTiempos[pasoActual] = millis();
  pasoActual++;
}

static bool obstacleBlocksForwardMotion(int leftFront, int rightFront, int leftRear, int rightRear) {
  const bool wantsForward = leftFront > 0 || rightFront > 0 || leftRear > 0 || rightRear > 0;
  return wantsForward && distancia_cm > 0 && distancia_cm <= STOP_DISTANCE_CM;
}

static void applyManualWheelSpeeds(int leftFront, int rightFront, int leftRear, int rightRear) {
  if (obstacleBlocksForwardMotion(leftFront, rightFront, leftRear, rightRear)) {
    stopMotors();
    obstacle_stop_active = true;
    last_command = 'x';
    sendTelemetry();
    return;
  }

  obstacle_stop_active = false;
  setWheelSpeeds(leftFront, rightFront, leftRear, rightRear);
  last_command = 'm';
  rememberAction('M');
  sendTelemetry();
}

static void applyManualSideSpeeds(int leftSpeed, int rightSpeed) {
  if (obstacleBlocksForwardMotion(leftSpeed, rightSpeed, leftSpeed, rightSpeed)) {
    stopMotors();
    obstacle_stop_active = true;
    last_command = 'x';
    sendTelemetry();
    return;
  }

  obstacle_stop_active = false;
  setSideSpeeds(leftSpeed, rightSpeed);
  last_command = 'm';
  rememberAction('M');
  sendTelemetry();
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

    case 'c':
      centerServo();
      break;

    case 'z':
      resetNanoState();
      break;

    case 't':
      // Prueba de vida manual: servo + ultrasonico + cada rueda de forma leve.
      runStartupSelfTest();
      last_command = 't';
      break;

    default:
      break;
  }

  sendTelemetry();
}

void setup() {
  // No iniciar Serial a 115200: D0/D1 estan fisicamente ocupados por el ESP32.
  // setupEsp32SerialLink() inicia Serial hardware a 9600.
  setupEsp32SerialLink();

  setupMotors();
  setupServo();
  setupDistanceSensor();

  resetNanoState();
  runStartupSelfTest();
}

void loop() {
  serviceServo();
  readCommandFromESP32();
  updateDistance();

  if (last_command == 'w' && distancia_cm > 0 && distancia_cm <= STOP_DISTANCE_CM) {
    stopMotors();
    obstacle_stop_active = true;
    last_command = 'x';
    sendTelemetry();
  }

  sendTelemetryEveryInterval();
  serviceServo();
}

void readCommandFromESP32() {
  static char lastProcessedCommand = 0;
  static unsigned long lastProcessedAt = 0;
  static bool inFrame = false;
  static char frameBuffer[80];
  static uint8_t frameIndex = 0;

  while (esp32Serial.available() > 0) {
    char command = esp32Serial.read();

    if (command == '<') {
      inFrame = true;
      frameIndex = 0;
      frameBuffer[0] = '\0';
      continue;
    }

    if (inFrame) {
      if (command == '>') {
        frameBuffer[frameIndex] = '\0';
        processFramedCommand(frameBuffer);
        inFrame = false;
        frameIndex = 0;
        continue;
      }

      if (command == '\n' || command == '\r') {
        inFrame = false;
        frameIndex = 0;
        continue;
      }

      if (frameIndex < sizeof(frameBuffer) - 1) {
        frameBuffer[frameIndex++] = command;
      } else {
        inFrame = false;
        frameIndex = 0;
      }
      continue;
    }

    if (command == '\n' || command == '\r' || command == ' ') {
      continue;
    }

    if (!(command == 'w' || command == 'a' || command == 's' || command == 'd' ||
          command == 'x' || command == 'q' || command == 'e' || command == 'c' || command == 'z' || command == 't')) {
      continue;
    }

    unsigned long now = millis();

    if (command == lastProcessedCommand && now - lastProcessedAt < 90) {
      continue;
    }

    lastProcessedCommand = command;
    lastProcessedAt = now;

    executeCommand(command);
  }
}

void processFramedCommand(const char* frame) {
  if (frame == nullptr || frame[0] == '\0') return;

  char op = '\0';
  int leftFront = 0;
  int rightFront = 0;
  int leftRear = 0;
  int rightRear = 0;
  int leftSide = 0;
  int rightSide = 0;
  int angle = SERVO_CENTER_ANGLE;

  // M = control por rueda: <M,lf,rf,lr,rr>
  // En esta configuracion final cada rueda se controla de forma independiente.
  if (sscanf(frame, " %c,%d,%d,%d,%d", &op, &leftFront, &rightFront, &leftRear, &rightRear) == 5) {
    if (op == 'M' || op == 'm') {
      applyManualWheelSpeeds(leftFront, rightFront, leftRear, rightRear);
      return;
    }
  }

  // B = solicitud por rueda + angulo de servo: <B,lf,rf,lr,rr,angle>
  if (sscanf(frame, " %c,%d,%d,%d,%d,%d", &op, &leftFront, &rightFront, &leftRear, &rightRear, &angle) == 6) {
    if (op == 'B' || op == 'b') {
      applyManualWheelSpeeds(leftFront, rightFront, leftRear, rightRear);
      setServoAngle(angle);
      sendTelemetry();
      return;
    }
  }

  // S = control directo por lado: <S,left,right>
  if (sscanf(frame, " %c,%d,%d", &op, &leftSide, &rightSide) == 3) {
    if (op == 'S' || op == 's') {
      applyManualSideSpeeds(leftSide, rightSide);
      return;
    }
  }

  // C = control por lado + servo: <C,left,right,angle>
  if (sscanf(frame, " %c,%d,%d,%d", &op, &leftSide, &rightSide, &angle) == 4) {
    if (op == 'C' || op == 'c') {
      applyManualSideSpeeds(leftSide, rightSide);
      setServoAngle(angle);
      sendTelemetry();
      return;
    }
  }

  // A = angulo absoluto de servo: <A,90>
  if (sscanf(frame, " %c,%d", &op, &angle) == 2) {
    if (op == 'A' || op == 'a') {
      setServoAngle(angle);
      last_command = 'g';
      sendTelemetry();
      return;
    }
  }
}

void resetNanoState() {
  stopMotors();

  motor_der = 0;
  motor_izq = 0;
  motor_izq_del = 0;
  motor_der_del = 0;
  motor_izq_tras = 0;
  motor_der_tras = 0;
  servo_angle = SERVO_CENTER_ANGLE;
  servo = SERVO_CENTER_ANGLE;
  distancia_cm = 0;

  centerServo();

  last_command = 'x';
  obstacle_stop_active = false;

  pasoActual = 0;
  modoReproduccion = false;

  for (int i = 0; i < MAX_PASOS; i++) {
    memoriaAcciones[i] = '\0';
    memoriaTiempos[i] = 0;
  }

  while (esp32Serial.available() > 0) {
    esp32Serial.read();
  }

  sendTelemetry();
}

void runStartupSelfTest() {
  if (!ENABLE_STARTUP_SELF_TEST) return;

  // Prueba visual de vida del servo.
  centerServo();
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_SERVO_MS);
  setServoAngle(SERVO_MIN_ANGLE);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_SERVO_MS);
  setServoAngle(SERVO_MAX_ANGLE);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_SERVO_MS);
  centerServo();
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_SERVO_MS);

  // Prueba de vida del ultrasonico: actualiza distancia y la manda al ESP32.
  distancia_cm = measureDistanceCm();
  sendTelemetry();
  delayKeepingServo(160);

  // Prueba de vida de motores por rueda real:
  // 1) delantera izquierda, 2) delantera derecha,
  // 3) trasera izquierda, 4) trasera derecha.
  setWheelSpeeds(STARTUP_TEST_SPEED, 0, 0, 0);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  sendTelemetry();
  delayKeepingServo(140);

  setWheelSpeeds(0, STARTUP_TEST_SPEED, 0, 0);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  sendTelemetry();
  delayKeepingServo(140);

  setWheelSpeeds(0, 0, STARTUP_TEST_SPEED, 0);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  sendTelemetry();
  delayKeepingServo(140);

  setWheelSpeeds(0, 0, 0, STARTUP_TEST_SPEED);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  sendTelemetry();
  delayKeepingServo(140);

  // Prueba general adelante y atras.
  setWheelSpeeds(STARTUP_TEST_SPEED, STARTUP_TEST_SPEED, STARTUP_TEST_SPEED, STARTUP_TEST_SPEED);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  sendTelemetry();
  delayKeepingServo(140);

  setWheelSpeeds(-STARTUP_TEST_SPEED, -STARTUP_TEST_SPEED, -STARTUP_TEST_SPEED, -STARTUP_TEST_SPEED);
  sendTelemetry();
  delayKeepingServo(STARTUP_TEST_MOTOR_MS);
  stopMotors();
  centerServo();
  sendTelemetry();
}
