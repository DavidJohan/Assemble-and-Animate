#include <ase/RotationController.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


#define 	DYNAMIXEL_MAX_POS_VALID 		1024 //1194
#define 	DYNAMIXEL_MAX_POS_FULL 			1194
static FILE *posLog;

static Rotation_t rotations[10];
static PIDController_t controllers[10];

Rotation_t* RotationController_getRotation(int actuator) {
	return &rotations[actuator];
}

void RotationController_setRotationControl(bool dir0, float frequency0, float phaseDiff0, int actuator) {
	rotations[actuator].dir = dir0;
	rotations[actuator].frequency = frequency0;
	rotations[actuator].phaseDiff = phaseDiff0;
	rotations[actuator].running = false;
	rotations[actuator].pause = false;
	rotations[actuator].changed = true;
}

void RotationController_setServoController(float P, float I, float D, int actuator) {
	controllers[actuator].P = P;
	controllers[actuator].I = I;
	controllers[actuator].D = D;
	controllers[actuator].avrSpeed = 0;
	controllers[actuator].previousError = 0;
	controllers[actuator].integral = 0;
	controllers[actuator].lastUpdateTime = getLocalTime();
}


int myAbs(int val) {
	return ((val<0)?-val:val);
}

void RotationController_applyRotation(int actuator) {
	float t = getLocalTime();
	float f =  rotations[actuator].frequency;
	float pd = rotations[actuator].phaseDiff/6.2831853f; //0-1
	float percentPos = f*t+pd;
	RotationController_applyRotationTo(actuator, percentPos);
}
static FILE* posLog;
void RotationController_logPos(int actuator, int cPos, int gPos) {
	if(posLog==0) {
		char name[100];
		sprintf(name, "RCSetPos.log");
		posLog = fopen(name,"w");
	}
	fprintf(posLog,"%f \t %i \t %i \t %i\n",getLocalTime(), actuator, cPos, gPos);
	fflush(posLog);
}
void RotationController_applyRotationTo(int actuator, float percentPos) {
	RotationController_setMode(actuator, true);
	float t = getLocalTime();
	int pos = dynamixelApi_getPosition(actuator);
	//float f =  rotations[actuator].frequency;
	//float pd = rotations[actuator].phaseDiff/6.28f; //0-1
	float dt = controllers[actuator].lastUpdateTime-t;

	//float percentPos;
	//percentPos = f*t+pd;
	int goalPos = ((int)(percentPos*DYNAMIXEL_MAX_POS_FULL))%DYNAMIXEL_MAX_POS_FULL;
	if(rotations[actuator].dir) goalPos = DYNAMIXEL_MAX_POS_FULL-goalPos;
	if(goalPos>DYNAMIXEL_MAX_POS_VALID) goalPos =DYNAMIXEL_MAX_POS_VALID;
	if(goalPos<0) goalPos = 0;
	int error = (goalPos-pos);

	RotationController_logPos(actuator, pos, goalPos);

	int speed;
	bool atValidAngle = dynamixelApi_isAtValidAngle(actuator, rotations[actuator].dir);
	if(atValidAngle) {
		if(rotations[actuator].dir  == true) {
			if(error>512){
				error = error-DYNAMIXEL_MAX_POS_VALID;
			}
			if(error<0 && error>-512) {
				controllers[actuator].integral = controllers[actuator].integral + myAbs(error)*dt;
				float derivative = (myAbs(error) - controllers[actuator].previousError)/dt;
				speed = controllers[actuator].P*myAbs(error) + controllers[actuator].I*controllers[actuator].integral + controllers[actuator].D*derivative;
				speed += controllers[actuator].avrSpeed;
				controllers[actuator].previousError = myAbs(error);
			}
			else {
				speed = -controllers[actuator].P*myAbs(error);
				speed += controllers[actuator].avrSpeed;
			}
		}
		else if(rotations[actuator].dir  == false) {
			if(error<-512){
				error = error+DYNAMIXEL_MAX_POS_VALID;
			}
			if(error>0&&error<512){
				controllers[actuator].integral = controllers[actuator].integral + myAbs(error)*dt;
				float derivative = (myAbs(error) - controllers[actuator].previousError)/dt;
				speed = controllers[actuator].P*myAbs(error) + controllers[actuator].I*controllers[actuator].integral + controllers[actuator].D*derivative;
				speed += controllers[actuator].avrSpeed;
				controllers[actuator].previousError = myAbs(error);
			}
			else {
				speed = -controllers[actuator].P*myAbs(error);
				speed += controllers[actuator].avrSpeed;
			}
		}

		if(speed>1023) speed = 1023;
		if(speed<=150) speed = 150;
		controllers[actuator].previousError = myAbs(error);
		rotations[actuator].speed = speed;
		rotations[actuator].changed = true;
	}
	else {
		speed = controllers[actuator].avrSpeed;// controllers[actuator].avrSpeed;
		if(speed<150) {
			speed = 150;
		}
		controllers[actuator].integral = 0;
		controllers[actuator].previousError = 0;
		rotations[actuator].speed = speed;
		rotations[actuator].changed = true;
		error = 0;
	}
	controllers[actuator].lastUpdateTime = t;
	if(!rotations[actuator].pause && rotations[actuator].running) {
		controllers[actuator].avrSpeed = (1.0-0.05)*controllers[actuator].avrSpeed+0.05*speed;
		if(rotations[actuator].changed ) {
			dynamixelApi_wheelMove(actuator, rotations[actuator].speed, rotations[actuator].dir);
			rotations[actuator].changed = false;
		}
	}
	else {
		if(rotations[actuator].pause && rotations[actuator].running) {
			float periodeTime = 1.0f/rotations[actuator].frequency;
			if(t-rotations[actuator].pauseStart > periodeTime) {
				rotations[actuator].pause = false;
			}
		}
		dynamixelApi_wheelMove(actuator, 0, rotations[actuator].dir);
	}
	int loopTimeMs=1000*(getLocalTime()-t);
	if(actuator==0||actuator==0) {
		//printf("%i: avr speed = %f, error = %i\n",actuator,controllers[actuator].avrSpeed, error);
	}
	if(loopTimeMs>20) {
		printf("%i: %i ms loop time (warning)\n",actuator,loopTimeMs);
	}
	//printf("{%i, %li, %i, %i, %i, %i, %i},\n",actuator,getLocalMsTime(),dynamixelApi_getPosition(actuator),dynamixelApi_getPositionEstimate(actuator,rotations[actuator].dir),goalPos,error,speed);
	//fprintf(posLog, "%i %li %i %i %i %i %i %i %f\n",actuator,getLocalMsTime(),dynamixelApi_getPosition(actuator),dynamixelApi_getPositionEstimate(actuator,rotations[actuator].dir),goalPos,error,speed,1023*(int)atValidAngle,controllers[actuator].avrSpeed);
	//fflush(posLog);
	//ase_printf("%i: Dynamixel at %i \n", i, pos);
}

/**
 * Change dynamixel mode if needed returns if changed
 */
bool RotationController_setMode(int actuator, bool wheel) {
	if(!dynamixelApi_isWheelMode(actuator)) {
		if(wheel) {
			dynamixelApi_setWheelMode(actuator, true);
			return true;
		}
	}
	else {
		if(!wheel) {
			dynamixelApi_setWheelMode(actuator, false);
			return true;
		}
	}
	return false;
}

void RotationController_initActuator(int actuator, bool wheel) {
	RotationController_setMode(actuator, wheel);
	float P = 0.9, I = 0, D = 0;
	RotationController_setServoController(P, I, D, actuator);
}

static int errorCount=0;
void error(char* topic, Event_t* event) {
	errorCount++;
	if(errorCount>10) {
		printf("Exiting due to Dynamixel error: %s \n", (char*)event->val_prt);
		//exit(1);
	}
}

void RotationController_initialize() {
	EventManager_subscribe(DYNAMIXEL_ERROR_EVENT,error);
}
