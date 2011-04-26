#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


typedef struct {
	bool changed;
	bool dir;
	int speed;
	float frequency;
	float phaseDiff;
	bool running;
	bool pause;
	float pauseStart;
} Rotation_t;

typedef struct {
	float P;
	float I;
	float D;
	float avrSpeed;
	float integral;
	float previousError;
	float lastUpdateTime;
} PIDController_t;

void RotationController_initialize();
void RotationController_initActuator(int actuator, bool wheel);
void RotationController_setServoController(float P, float I, float D, int actuator);
void RotationController_setRotationControl(bool dir0, float frequency0, float phaseDiff0, int actuator);
void RotationController_applyRotation(int actuator);
void RotationController_applyRotationTo(int actuator, float percentPos);
Rotation_t* RotationController_getRotation(int actuator);
bool RotationController_setMode(int actuator, bool wheel);
