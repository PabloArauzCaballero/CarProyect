#include "Constants.h"
#include "Globals.h"

int motor_der = 0;
int motor_izq = 0;
int servo_angle = SERVO_CENTER_ANGLE;
int distancia_cm = 0;

unsigned long lastTelemetrySent = 0;
unsigned long lastDistanceUpdate = 0;
