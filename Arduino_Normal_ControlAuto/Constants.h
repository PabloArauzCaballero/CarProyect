#pragma once

static constexpr unsigned long ESP32_SERIAL_BAUD = 115200;

// Ajusta estos pines a tu driver real.
static constexpr int MOTOR_RIGHT_EN  = 5;
static constexpr int MOTOR_RIGHT_IN1 = 6;
static constexpr int MOTOR_RIGHT_IN2 = 7;

static constexpr int MOTOR_LEFT_EN   = 10;
static constexpr int MOTOR_LEFT_IN1  = 8;
static constexpr int MOTOR_LEFT_IN2  = 9;

static constexpr int DEFAULT_MOTOR_SPEED = 180;

static constexpr int SERVO_PIN = 11;
static constexpr int SERVO_CENTER_ANGLE = 90;
static constexpr int SERVO_MIN_ANGLE = 30;
static constexpr int SERVO_MAX_ANGLE = 150;
static constexpr int SERVO_STEP = 15;

static constexpr int ULTRASONIC_TRIG_PIN = 12;
static constexpr int ULTRASONIC_ECHO_PIN = 13;
static constexpr unsigned long ULTRASONIC_TIMEOUT_US = 30000;

static constexpr unsigned long TELEMETRY_INTERVAL_MS = 1000;
static constexpr unsigned long DISTANCE_UPDATE_INTERVAL_MS = 150;
