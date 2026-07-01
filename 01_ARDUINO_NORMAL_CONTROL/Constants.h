#pragma once
#include <Arduino.h>

// =========================================================
// CONFIGURACION FISICA FINAL 4WD INDEPENDIENTE
// =========================================================
// Este archivo respeta exactamente la nueva configuracion fisica:
//
// ESP32 GPIO17 / TX2 -> Nano D0 / RX
// ESP32 GPIO16 / RX2 <- Nano D1 / TX mediante divisor de voltaje
// GND ESP32          <-> GND Nano
//
// IMPORTANTE PARA SUBIR EL SKETCH AL NANO:
// D0/D1 son Serial hardware y tambien se usan para cargar codigo por USB.
// Si falla la carga, desconecta temporalmente los cables ESP32<->Nano de D0/D1,
// sube el sketch y vuelve a conectarlos.
static constexpr unsigned long ESP32_SERIAL_BAUD = 9600;
static constexpr int NANO_RX_FROM_ESP32_PIN = 0;  // D0 / RX hardware
static constexpr int NANO_TX_TO_ESP32_PIN = 1;    // D1 / TX hardware

// =========================================================
// SERVO Y ULTRASONICO
// =========================================================
static constexpr int PIN_SERVO = 9;   // Servo base en D9
static constexpr int trigger = A2;    // Ultrasonico Trigger en A2 como digital
static constexpr int echo = A3;       // Ultrasonico Echo en A3 como digital

// =========================================================
// PLACA DELANTERA - MOTORES 1 Y 2
// =========================================================
// Delantero izquierdo
static constexpr int moti1_A = 2;     // IN1 Delantero Izquierdo -> D2
static constexpr int veli1  = 3;      // ENA Velocidad Del. Izq. -> D3 PWM
static constexpr int moti1_B = 4;     // IN2 Delantero Izquierdo -> D4

// Delantero derecho
static constexpr int veld1  = 5;      // ENB Velocidad Del. Der. -> D5 PWM
static constexpr int motd1_A = 7;     // IN3 Delantero Derecho -> D7
static constexpr int motd1_B = 8;     // IN4 Delantero Derecho -> D8

// =========================================================
// PLACA TRASERA - MOTORES 3 Y 4
// =========================================================
// Trasero izquierdo
static constexpr int veli2  = 10;     // ENA Velocidad Tras. Izq. -> D10 PWM
static constexpr int moti2_A = 12;    // IN1 Trasero Izquierdo -> D12
static constexpr int moti2_B = 13;    // IN2 Trasero Izquierdo -> D13

// Trasero derecho
static constexpr int veld2  = 11;     // ENB Velocidad Tras. Der. -> D11 PWM
static constexpr int motd2_A = A0;    // IN3 Trasero Derecho -> A0 como digital
static constexpr int motd2_B = A1;    // IN4 Trasero Derecho -> A1 como digital

// Nota tecnica importante corregida:
// NO se usa la libreria Servo clasica para evitar conflicto de Timer1 con PWM en D10.
// El servo se controla por pulsos de software en D9 y D10 queda libre para PWM real.

// Variables de ajuste originales
static constexpr int VELOCIDAD = 180;
static constexpr int VELOCIDAD_GIRO = 210;
static constexpr int DISTANCIA_PARADA = 20;
static constexpr unsigned long TIEMPO_GIRO_90 = 600UL;
static constexpr int MAX_PASOS = 60;

// =========================================================
// ALIASES PARA EL CODIGO MODULAR, SIN BORRAR TUS NOMBRES
// =========================================================
static constexpr int SERVO_PIN = PIN_SERVO;
static constexpr int ULTRASONIC_TRIG_PIN = trigger;
static constexpr int ULTRASONIC_ECHO_PIN = echo;
static constexpr unsigned long ULTRASONIC_TIMEOUT_US = 18000UL;

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

// Prueba de vida al encender y bajo comando t desde la web.
static constexpr bool ENABLE_STARTUP_SELF_TEST = true;
static constexpr int STARTUP_TEST_SPEED = 205;   // Suficiente para vencer friccion inicial.
static constexpr unsigned long STARTUP_TEST_MOTOR_MS = 650UL;
static constexpr unsigned long STARTUP_TEST_SERVO_MS = 450UL;

// Control manual desde ESP32.
static constexpr int MANUAL_PWM_MIN = -255;
static constexpr int MANUAL_PWM_MAX = 255;

// Servo por software, sin libreria Servo.h.
static constexpr unsigned int SERVO_MIN_US = 544;
static constexpr unsigned int SERVO_MAX_US = 2400;
static constexpr unsigned long SERVO_PERIOD_US = 20000UL;
