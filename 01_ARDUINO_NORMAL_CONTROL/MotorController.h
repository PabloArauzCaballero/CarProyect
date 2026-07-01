#pragma once

void setupMotors();
void setSideSpeeds(int leftSpeed, int rightSpeed);
void setWheelSpeeds(int leftFront, int rightFront, int leftRear, int rightRear);
void moveForward();
void moveBackward();
void turnLeft();
void turnRight();
void stopMotors();
