#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>
#include <pthread.h>
typedef struct {
	bool changed;
	bool dir;
	int speed;
	float frequency;
	float phaseDiff;
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

Rotation_t rotations[10];
PIDController_t controllers[10];

int state = 0;
int delay = 0;
long lastUpdate = 0;
bool rotationStyle = true;
int initialized = 0;
FILE *posLog;
#define 	DYNAMIXEL_MAX_POS_VALID 		1024 //1194
#define 	DYNAMIXEL_MAX_POS_FULL 			1194

void openLog() {
  posLog = fopen("x:\\Desktop\\pos.log", "w");
  if (posLog == NULL ) {
	printf("Error: I couldn't open %s for writing.\n","pos.log");
  }
}

int myAbs(int val) {
	return ((val<0)?-val:val);
}
void applyRotation(int actuator) {
	float t = getLocalTime();
	//int pos = dynamixelApi_getPositionEstimate(actuator, rotations[actuator].dir);
	int pos = dynamixelApi_getPosition(actuator);
	float f =  rotations[actuator].frequency;
	float pd = rotations[actuator].phaseDiff/6.28f; //0-1
	float dt = controllers[actuator].lastUpdateTime-t;
	//if(actuator==1) printf("%f sec\n",t);
	float percentPos;
	percentPos = f*t+pd;
	int goalPos = ((int)(percentPos*DYNAMIXEL_MAX_POS_FULL))%DYNAMIXEL_MAX_POS_FULL;
	if(rotations[actuator].dir) goalPos = DYNAMIXEL_MAX_POS_FULL-goalPos;
	if(goalPos>DYNAMIXEL_MAX_POS_VALID) goalPos =DYNAMIXEL_MAX_POS_VALID;
	if(goalPos<0) goalPos = 0;
	int error = (goalPos-pos);
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
	if(!rotations[actuator].pause) {
		controllers[actuator].avrSpeed = (1.0-0.05)*controllers[actuator].avrSpeed+0.05*speed;
		if(rotations[actuator].changed ) {
			dynamixelApi_wheelMove(actuator, rotations[actuator].speed, rotations[actuator].dir);
			rotations[actuator].changed = false;
		}
	}
	else {
		float periodeTime = 1.0f/rotations[actuator].frequency;
		if(t-rotations[actuator].pauseStart > periodeTime) {
			rotations[actuator].pause = false;
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

void rotationControl() {
	if(state==1) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			if(!dynamixelApi_isWheelMode(i)) {
				dynamixelApi_setWheelMode(i, true);
			}
		}
		state++;
	}
	else if(state==2) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			//dynamixelApi_wheelMove(i, 1023, false);
			applyRotation(i);
		}
	}
	delay = 0;
}

void cpgControl() {
	if(state==1) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			CpgGait_setAmplitude(0.5f, i);
			CpgGait_setOffset(0.5f, i);
			CpgGait_setFrequency(1.0f, i);
			CpgGait_setUseFeedback(0);
			CpgGait_setPhaseShift(0, i);
		}
		if(!CpgGait_isStarted()) {
			CpgGait_start();
		}
		delay = 100;	state++;
	}
	else if(state==2) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			ase_printf("%i: Dynamixel at %i\n", i, dynamixelApi_getPosition(i));
		}
		delay = 100;
	}
}
void act(char* topic, Event_t* event)  {
	if(getLocalMsTime()<=(delay+lastUpdate)) {
		return;
	}
	else {
		lastUpdate = getLocalMsTime();
	}
	if(state==0) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			dynamixelApi_setGoalPos(i,512);
		}
		delay = 2000;	state++;
	}
	else {
		if(rotationStyle) {
			rotationControl();
			return;
		}
		else {
			cpgControl();
			return;
		}
	}
}

void handleMessage(char* topic, Event_t* event) { }

void setRotationControl(bool dir0, float frequency0, float phaseDiff0, int actuator) {
	rotations[actuator].dir = dir0;
	rotations[actuator].frequency = frequency0;
	rotations[actuator].phaseDiff = phaseDiff0;
	rotations[actuator].pause = false;
	rotations[actuator].changed = true;
}

void setServoController(float P, float I, float D, int actuator) {
	controllers[actuator].P = P;
	controllers[actuator].I = I;
	controllers[actuator].D = D;
	controllers[actuator].avrSpeed = 0;
	controllers[actuator].previousError = 0;
	controllers[actuator].integral = 0;
	controllers[actuator].lastUpdateTime = getLocalTime();
}

int errorCount=0;
void error(char* topic, Event_t* event) {
	errorCount++;
	if(errorCount>10) {
		printf("Exiting due to Dynamixel error: %s \n", (char*)event->val_prt);
		exit(1);
	}
}

void *listenForKeyInput() {
	while(1) {
		char command;
		printf("Type in a command: \n");
		printf("	p - pause for one period\n");
		printf("	q - turn left (pause left for one period)\n");
		printf("	e - turn right (pause right for one period)\n");
		printf("	f - Increase frequency\n");
		printf("	g - Decrease frequency\n");
		printf("	h - Increase lift  modules phase-shift\n");
		printf("	j - Decrease lift modules phase-shift\n");
		printf("	k - Increase stride modules phase-shift\n");
		printf("	l - Decrease stride modules phase-shift\n");
		scanf("%c", &command);
		printf("The command you typed was %c\n", command);
		int i;
		if(command=='p') {
			for(i=0;i<getNumberOfActuators();i++) {
				rotations[i].pause = true;
				rotations[i].pauseStart = getLocalTime();
			}
		}
		//6,7,8,9
		if(command=='q') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==4||i==5||i==6||i==7) {
					rotations[i].pause = true;
					rotations[i].pauseStart = getLocalTime();
				}
			}
		}
		if(command=='e') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0||i==1||i==2||i==3) {
					rotations[i].pause = true;
					rotations[i].pauseStart = getLocalTime();
				}
			}
		}
		if(command=='f'||command=='g') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(command=='f') rotations[i].frequency += 0.1f;
				if(command=='g') rotations[i].frequency -= 0.1f;
			}
			printf("New frequency = %f\n",rotations[0].frequency);
		}
		//stride modules: 4=>2,3=>1,8=>6,7=>5
		//lift modules: 2=>0,5=>3,6=>4,9=>7
		if(command=='h'||command=='j') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0||i==3||i==4||i==7) {
					if(command=='h') rotations[i].phaseDiff += 0.1f;
					if(command=='j') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for lift modules = %f\n",rotations[0].phaseDiff);
		}
		if(command=='k'||command=='l') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==2||i==1||i==6||i==5) {
					if(command=='k') rotations[i].phaseDiff += 0.1f;
					if(command=='l') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for stride modules = %f\n",rotations[0].phaseDiff);
		}
	}
}
void startKeyInputControl() {
	pthread_t listenThread;
	pthread_create(&listenThread, NULL, listenForKeyInput, NULL);
}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	startKeyInputControl();


	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	EventManager_subscribe(DYNAMIXEL_ERROR_EVENT,error);

	openLog();
	//dynamixelApi_connect(1);
	dynamixelApi_connect(2);
	dynamixelApi_connect(3);
	dynamixelApi_connect(4);
	dynamixelApi_connect(5);
	dynamixelApi_connect(6);
	dynamixelApi_connect(7);
	dynamixelApi_connect(8);
	dynamixelApi_connect(9);

	if(!rotationStyle) {
		CpgGait_init();
		CpgGait_setCouplingStrenghts(0.5f, 0.5f, 0.5f);
	}
	else {
		float freq = 0.7f;
		float pi= 3.14;
		float pih = pi/2;

	//All motors in sync
	/*	setRotationControl(false, freq, 0, 0); //2
		setRotationControl(false, freq, 0, 1); //3

		setRotationControl(false, freq, pi, 2); //4
		setRotationControl(false, freq, pi, 3); //5

		setRotationControl(true, freq, pi, 4); //6 -0,5
		setRotationControl(true, freq, pi, 5); //7

		setRotationControl(true, freq, 0, 6); //8
		setRotationControl(true, freq, 0, 7); //9 */

/*		setRotationControl(false, freq, pi, 0); //2
		setRotationControl(false, freq, pi, 1); //3

		setRotationControl(false, freq, pi, 2); //4
		setRotationControl(false, freq, pi, 3); //5

		setRotationControl(true, freq, 2*pi, 4); //6 -0,5
		setRotationControl(true, freq, 2*pi, 5); //7

		setRotationControl(true, freq, 0, 6); //8
		setRotationControl(true, freq, 0, 7); //9 */

		//stride modules: 4=>2,3=>1,8=>6,7=>5
		//lift modules: 2=>0,5=>3,6=>4,9=>7
		int gait = 0;
		if(gait==0) {
			setRotationControl(false, freq, -1, 0); //2
			setRotationControl(false, freq, -1, 1); //3

			setRotationControl(false, freq, 0, 2); //4
			setRotationControl(false, freq, 0, 3); //5

			setRotationControl(true, freq, pi, 4); //6
			setRotationControl(true, freq, pi, 5); //7

			setRotationControl(true, freq, pi, 6); //8
			setRotationControl(true, freq, pi, 7); //9
		}
		if(gait==1) {
			float freq = 1.0f;
			//Singlefoot (lateral sequence
			setRotationControl(false, freq, pi, 0); //2
			setRotationControl(false, freq, pi, 1); //3

			setRotationControl(false, freq, pih*3, 2); //4
			setRotationControl(false, freq, (pih*3), 3); //5

			setRotationControl(true, freq, pih, 4); //6 -0,5
			setRotationControl(true, freq, pih+0.3, 5); //7

			setRotationControl(true, freq, 0, 6); //8
			setRotationControl(true, freq, 0, 7); //9
		}
		if(gait==2) {
			setRotationControl(true, freq, 0-(1)+pi, 0); //2
			setRotationControl(false, freq, 0-(1), 1); //3

			setRotationControl(false, freq, (0), 2); //4
			setRotationControl(true, freq, (0)+pi, 3); //5

			setRotationControl(false, freq, (0+pi)+pi, 4); //6
			setRotationControl(true, freq, (0+pi), 5); //7

			setRotationControl(true, freq, pi, 6); //8
			setRotationControl(false, freq, pi+pi, 7); //9
		}

		int i;
		for(i=0;i<10;i++) {
			float P = 0.9, I = 0, D = 0;
			setServoController(P, I, D, i);
		}
	}
}

