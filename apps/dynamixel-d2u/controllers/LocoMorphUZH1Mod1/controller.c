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

//default gait
int gait = 1;

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

void gait_selection()
{
	float freq = 1.1f;
	float pi = 3.14f;
	float offsetR = -1.97079633;

	float offset_LF = 0;
	float offset_LH = 2.54f;
	float offset_RF = 0;
	float offset_RH = 2.64f;

	if(gait==0) {
		//Swing Platform
		setRotationControl(true,
							freq,
							offset_LF,
							0); //2 LF

		setRotationControl(true,
							freq,
							offset_LH,
							1); //3 LH

		setRotationControl(false,
							freq,
							offset_RF,
							2); //4 RF

		setRotationControl(false,
							freq,
							offset_RH,
							3); //5 RH
	}
	if(gait==1) {
		//Trot gait
		setRotationControl(true,
							freq,
							offset_LF,
							0); //2 LF

		setRotationControl(true,
							freq,
							offset_LH,
							1); //3 LH

		setRotationControl(false,
							freq,
							offset_RF,
							2); //4 RF

		setRotationControl(false,
							freq,
							offset_RH,
							3); //5 RH
	}
	if(gait==2) {
		//????
		setRotationControl(true, freq, (pi/2)+offsetR, 0); //2 LF
		setRotationControl(true, freq, 0+offsetR, 1); //3 LH
		setRotationControl(false, freq, ((pi*3)/2), 2); //4 RF
		setRotationControl(false, freq, pi, 3); //5 RH
	}
	if(gait==3) {
		//Bounding gait
		setRotationControl(true, freq, 0+offsetR, 0); //2 LF
		setRotationControl(true, freq, pi+offsetR, 1); //3 LH
		setRotationControl(false, freq, 0, 2); //4 RF
		setRotationControl(false, freq, pi, 3); //5 RH
	}

	int i;
	for(i=0;i<10;i++) {
		float P = 0.9, I = 0, D = 0;
		setServoController(P, I, D, i);
	}
}

void *listenForKeyInput() {
	while(1) {
		char command;
		printf("Type in a command: \n");
		printf("	0 - Gait: Swing Platform\n");
		printf("	1 - Gait: Trot gait\n");
		printf("	2 - Gait: Lateral rotation 2\n");
		printf("	3 - Gait: Bounding gait\n");
		printf("	p - pause the robot\n");
		printf("	s - starts the robot\n");
		printf("	q - turn left (pause left for one period)\n");
		printf("	e - turn right (pause right for one period)\n");
		printf("	f - Increase frequency\n");
		printf("	g - Decrease frequency\n");
		printf("	h - Increase LH phase-shift\n");
		printf("	i - Decrease LH phase-shift\n");
		printf("	j - Increase LF phase-shift\n");
		printf("	k - Decrease LF phase-shift\n");
		printf("	l - Increase RH phase-shift\n");
		printf("	m - Decrease RH phase-shift\n");
		printf("	n - Increase RF phase-shift\n");
		printf("	o - Decrease RF phase-shift\n");
		printf("	a - Print out values\n");
		scanf("%c", &command);
		printf("The command you typed was %c\n", command);
		int i;
		if(command=='p') {
			for(i=0;i<getNumberOfActuators();i++) {
				rotations[i].pause = true;
				rotations[i].pauseStart = getLocalTime();
			}
		}

		if(command=='s') {
			for(i=0;i<getNumberOfActuators();i++) {
				rotations[i].pause = false;
				rotations[i].pauseStart = getLocalTime();
			}
		}

		if(command=='q') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==2||i==3) {
					rotations[i].pause = true;
					rotations[i].pauseStart = getLocalTime();
				}
			}
		}
		if(command=='e') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0||i==1) {
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

		if(command=='h'||command=='i') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==1) {
					if(command=='h') rotations[i].phaseDiff += 0.1f;
					if(command=='i') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for LH module = %f\n",rotations[0].phaseDiff);
		}
		if(command=='j'||command=='k') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0) {
					if(command=='j') rotations[i].phaseDiff += 0.1f;
					if(command=='k') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for LF module = %f\n",rotations[0].phaseDiff);
		}
		if(command=='l'||command=='m') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==3) {
					if(command=='l') rotations[i].phaseDiff += 0.1f;
					if(command=='m') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for RH module = %f\n",rotations[0].phaseDiff);
		}
		if(command=='n'||command=='o') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==2) {
					if(command=='n') rotations[i].phaseDiff += 0.1f;
					if(command=='o') rotations[i].phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for RF module = %f\n",rotations[0].phaseDiff);
		}
		if(command=='0') {
			gait = 0;
			gait_selection();
		}
		if(command=='1') {
			gait = 1;
			gait_selection();
		}
		if(command=='2') {
			gait = 2;
			gait_selection();
		}
		if(command=='3') {
			gait = 3;
			gait_selection();
		}

		if(command=='a') {
				printf("Phase-shift for RF module = %f\n",rotations[2].phaseDiff);
				printf("Phase-shift for LF module = %f\n",rotations[0].phaseDiff);
				printf("Phase-shift for RH module = %f\n",rotations[3].phaseDiff);
				printf("Phase-shift for LH module = %f\n",rotations[1].phaseDiff);
				printf("Frequency = %f\n",rotations[0].frequency);
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

	//openLog();
	dynamixelApi_connect(2);
	dynamixelApi_connect(3);
	dynamixelApi_connect(4);
	dynamixelApi_connect(5);

	if(!rotationStyle) {
		CpgGait_init();
		CpgGait_setCouplingStrenghts(0.5f, 0.5f, 0.5f);
	}
	else {
		gait_selection();
	}
}

