#pragma once
#include <Arduino.h>

// =========================================================
// COMUNICACION NANO <-> ESP32, SEGUN TU NUEVO CABLEADO
// =========================================================
// Cableado indicado por ti:
//   ESP32 RX2  <- Nano D10  (D10 es TX del Nano hacia el ESP32)
//   ESP32 TX2  -> Nano D13  (D13 es RX del Nano desde el ESP32)
//   GND        <-> GND
//
// Como el Nano NO tiene Serial hardware en D10/D13, se usa SoftwareSerial.
// BAUD ESTABLE: SoftwareSerial en Arduino Nano con D10/D13 no es confiable a 115200.
// Se usa 9600 para que los paquetes lleguen limpios y no se llene el buffer.
static constexpr unsigned long ESP32_SERIAL_BAUD = 9600;
static constexpr int NANO_RX_FROM_ESP32_PIN = 13;
static constexpr int NANO_TX_TO_ESP32_PIN = 10;

// =========================================================
// VARIABLES/PINES ORIGINALES QUE PEDISTE CONSERVAR
// =========================================================
// PINES DE SENSORES Y SERVO
static constexpr int PIN_SERVO = A3;
static constexpr int trigger = A5;
static constexpr int echo = A4;

// PUENTE H #1 Delantero
static constexpr int moti1_A = 4;
static constexpr int moti1_B = 2;
static constexpr int veli1 = 5;
static constexpr int motd1_A = 9;
static constexpr int motd1_B = 7;
static constexpr int veld1 = 3;

// PUENTE H #2 Trasero
static constexpr int moti2_A = A1;
static constexpr int moti2_B = A0;
static constexpr int veli2 = 6;
static constexpr int motd2_A = 8;
static constexpr int motd2_B = 12;
static constexpr int veld2 = 11;

// Variables de ajuste originales
static constexpr int VELOCIDAD = 180;
static constexpr int VELOCIDAD_GIRO = 255;
static constexpr int DISTANCIA_PARADA = 20;
static constexpr unsigned long TIEMPO_GIRO_90 = 600UL;
static constexpr int MAX_PASOS = 60;

// =========================================================
// ALIASES PARA EL CODIGO MODULAR, SIN BORRAR TUS NOMBRES
// =========================================================
static constexpr int SERVO_PIN = PIN_SERVO;
static constexpr int ULTRASONIC_TRIG_PIN = trigger;
static constexpr int ULTRASONIC_ECHO_PIN = echo;
static constexpr unsigned long ULTRASONIC_TIMEOUT_US = 30000UL;

static constexpr int MOTI1_A = moti1_A;
static constexpr int MOTI1_B = moti1_B;
static constexpr int VELI1  = veli1;

static constexpr int MOTD1_A = motd1_A;
static constexpr int MOTD1_B = motd1_B;
static constexpr int VELD1  = veld1;

static constexpr int MOTI2_A = moti2_A;
static constexpr int MOTI2_B = moti2_B;
static constexpr int VELI2  = veli2;

static constexpr int MOTD2_A = motd2_A;
static constexpr int MOTD2_B = motd2_B;
static constexpr int VELD2  = veld2;

static constexpr int DEFAULT_MOTOR_SPEED = VELOCIDAD;
static constexpr int TURN_MOTOR_SPEED = VELOCIDAD_GIRO;
static constexpr int STOP_DISTANCE_CM = DISTANCIA_PARADA;
static constexpr unsigned long TURN_90_TIME_MS = TIEMPO_GIRO_90;

// Servo.
static constexpr int SERVO_CENTER_ANGLE = 90;
static constexpr int SERVO_MIN_ANGLE = 30;
static constexpr int SERVO_MAX_ANGLE = 150;
static constexpr int SERVO_STEP = 15;

// Telemetria.
static constexpr unsigned long TELEMETRY_INTERVAL_MS = 250UL;
static constexpr unsigned long DISTANCE_UPDATE_INTERVAL_MS = 100UL;
