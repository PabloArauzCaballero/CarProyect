#include <Servo.h>
#include "Constants.h"
#include "Globals.h"

Servo myservo;

int motor_der = 0;
int motor_izq = 0;
int servo_angle = SERVO_CENTER_ANGLE;
int distancia_cm = 0;

char last_command = 'x';
bool obstacle_stop_active = false;

char memoriaAcciones[MAX_PASOS];
unsigned long memoriaTiempos[MAX_PASOS];
int pasoActual = 0;
bool modoReproduccion = false;

unsigned long lastTelemetrySent = 0;
unsigned long lastDistanceUpdate = 0;
