#pragma once
#include <Servo.h>

// Servo global original que pediste conservar.
extern Servo myservo;

extern int motor_der;
extern int motor_izq;
extern int servo_angle;
extern int distancia_cm;

extern char last_command;
extern bool obstacle_stop_active;

extern char memoriaAcciones[];
extern unsigned long memoriaTiempos[];
extern int pasoActual;
extern bool modoReproduccion;

extern unsigned long lastTelemetrySent;
extern unsigned long lastDistanceUpdate;
