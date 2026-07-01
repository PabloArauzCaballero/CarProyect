#pragma once

// Sin Servo.h: el servo se controla por pulsos de software para no romper PWM en D10.

extern int servo;
extern int motor_der;
extern int motor_izq;
extern int motor_izq_del;
extern int motor_der_del;
extern int motor_izq_tras;
extern int motor_der_tras;
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
