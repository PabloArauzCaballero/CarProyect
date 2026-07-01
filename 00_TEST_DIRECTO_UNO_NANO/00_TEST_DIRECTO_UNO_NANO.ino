/*
  TEST DIRECTO ARDUINO UNO / NANO - ROBOT 4WD
  ------------------------------------------------------------
  Este sketch NO depende del ESP32, WiFi, HTML, MQTT ni Serial.
  Sirve para saber si el problema es codigo/comunicacion o fisico.

  Carga este sketch al Arduino UNO/NANO y deja el robot elevado.
  Al encender debe:
  1) Mover el servo.
  2) Probar cada rueda por separado.
  3) Probar todos los motores adelante y atras.
  4) Repetir el ciclo.

  IMPORTANTE:
  - Si este test no mueve nada, el problema NO es el HTML.
  - Revisa bateria de motores, GND comun, ENA/ENB, IN1-IN4, driver, switch, alimentacion.
*/

// Servo y ultrasonico
const int PIN_SERVO = 9;
const int TRIG_PIN = A2;
const int ECHO_PIN = A3;

// Placa delantera
const int FL_IN_A = 2;
const int FL_PWM  = 3;
const int FL_IN_B = 4;

const int FR_PWM  = 5;
const int FR_IN_A = 7;
const int FR_IN_B = 8;

// Placa trasera
const int RL_PWM  = 10;
const int RL_IN_A = 12;
const int RL_IN_B = 13;

const int RR_PWM  = 11;
const int RR_IN_A = A0;
const int RR_IN_B = A1;

const int TEST_PWM = 220;
const unsigned long MOTOR_MS = 750;
const unsigned long GAP_MS = 350;

const unsigned int SERVO_MIN_US = 544;
const unsigned int SERVO_MAX_US = 2400;
const unsigned long SERVO_PERIOD_US = 20000UL;

int servoAngle = 90;
unsigned int servoPulseWidthUs = 1500;
unsigned long lastServoPulseAtUs = 0;

int clampPwm(int value) {
  if (value > 255) return 255;
  if (value < -255) return -255;
  return value;
}

void serviceServo() {
  unsigned long nowUs = micros();
  if (nowUs - lastServoPulseAtUs < SERVO_PERIOD_US) return;

  lastServoPulseAtUs = nowUs;
  digitalWrite(PIN_SERVO, HIGH);
  delayMicroseconds(servoPulseWidthUs);
  digitalWrite(PIN_SERVO, LOW);
}

void waitServo(unsigned long ms) {
  unsigned long start = millis();
  while (millis() - start < ms) {
    serviceServo();
    delay(2);
  }
}

void setServoAngle(int angle) {
  if (angle < 30) angle = 30;
  if (angle > 150) angle = 150;
  servoAngle = angle;
  servoPulseWidthUs = map(angle, 0, 180, SERVO_MIN_US, SERVO_MAX_US);
  waitServo(250);
}

void setMotor(int inA, int inB, int pwmPin, int speedValue) {
  speedValue = clampPwm(speedValue);
  int pwm = abs(speedValue);

  if (speedValue > 0) {
    digitalWrite(inA, HIGH);
    digitalWrite(inB, LOW);
  } else if (speedValue < 0) {
    digitalWrite(inA, LOW);
    digitalWrite(inB, HIGH);
  } else {
    digitalWrite(inA, LOW);
    digitalWrite(inB, LOW);
  }

  analogWrite(pwmPin, pwm);
}

void setWheels(int lf, int rf, int lr, int rr) {
  setMotor(FL_IN_A, FL_IN_B, FL_PWM, lf);
  setMotor(FR_IN_A, FR_IN_B, FR_PWM, rf);
  setMotor(RL_IN_A, RL_IN_B, RL_PWM, lr);
  setMotor(RR_IN_A, RR_IN_B, RR_PWM, rr);
}

void stopAll() {
  setWheels(0, 0, 0, 0);
}

long measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(ECHO_PIN, HIGH, 18000UL);
  if (duration == 0) return -1;
  return (long)(duration / 58UL);
}

void testOneWheel(int lf, int rf, int lr, int rr) {
  setWheels(lf, rf, lr, rr);
  waitServo(MOTOR_MS);
  stopAll();
  waitServo(GAP_MS);
}

void setup() {
  pinMode(PIN_SERVO, OUTPUT);
  digitalWrite(PIN_SERVO, LOW);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(FL_IN_A, OUTPUT);
  pinMode(FL_IN_B, OUTPUT);
  pinMode(FL_PWM, OUTPUT);

  pinMode(FR_IN_A, OUTPUT);
  pinMode(FR_IN_B, OUTPUT);
  pinMode(FR_PWM, OUTPUT);

  pinMode(RL_IN_A, OUTPUT);
  pinMode(RL_IN_B, OUTPUT);
  pinMode(RL_PWM, OUTPUT);

  pinMode(RR_IN_A, OUTPUT);
  pinMode(RR_IN_B, OUTPUT);
  pinMode(RR_PWM, OUTPUT);

  stopAll();
  waitServo(1200);
}

void loop() {
  // Servo: izquierda-centro-derecha-centro.
  setServoAngle(30);
  setServoAngle(90);
  setServoAngle(150);
  setServoAngle(90);

  // Ultrasonico: solo se mide para validar que no bloquea el flujo.
  measureDistanceCm();
  waitServo(250);

  // Ruedas adelante, una por una.
  testOneWheel(TEST_PWM, 0, 0, 0);       // Delantera izquierda
  testOneWheel(0, TEST_PWM, 0, 0);       // Delantera derecha
  testOneWheel(0, 0, TEST_PWM, 0);       // Trasera izquierda
  testOneWheel(0, 0, 0, TEST_PWM);       // Trasera derecha

  // Todas adelante y todas atras.
  testOneWheel(TEST_PWM, TEST_PWM, TEST_PWM, TEST_PWM);
  testOneWheel(-TEST_PWM, -TEST_PWM, -TEST_PWM, -TEST_PWM);

  stopAll();
  waitServo(2500);
}
