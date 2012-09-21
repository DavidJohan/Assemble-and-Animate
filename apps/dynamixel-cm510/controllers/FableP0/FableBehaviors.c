#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>
#include "controller.h"
#include "FableBehaviors.h"
static long itt=0;

void playback_start(void* data){
	Playback_startPlayback(data);
}

void playback_stop(void* data){};
void playback_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	signed char playbackOutput[nOutputs];
	Playback_getOutput((Playback_t*) data, playbackOutput, nOutputs);
	bool applyControl = false;
	for(int i=0;i<nOutputs;i++) {
		if(playbackOutput[i]!=0) {
			applyControl = true;
		}
	}
	if(applyControl) {
		for(int i=0;i<nOutputs;i++) {
			output[i] = playbackOutput[i];
		}
	}
}

void osscilate_start(void* data) { }

void osscilate_stop(void* data) {}
void osscilate_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	if(getLocalMsTime()<10*1000) {
		for(int i=0;i<nOutputs;i++) {
			output[i] = 100*(sin(6*getLocalTime()));
		}
	}
	else output[0] = 0;
}


//2 actuators, 32 bits per integer
//25 samples per second
//100 bytes per second
//1k byte per 5 second
/*typedef struct {
	int min;
	int max;
	long minT;
	long maxT;
} PhysicalProgrammingCPG_t;

PhysicalProgrammingCPG_t ppData[2];
static void physicalProgramCPG(int pos, int index) {
	long time = getLocalMsTime();
	if(pos<ppData[index].min) {
		ppData[index].min = pos;
		ppData[index].minT = time;
	}
	if(ppData[index].max>pos) {
		ppData[index].max = pos;
		ppData[index].minT = time;
	}
}

float phaseShift_upper 	= 0.0f;
float amplitude_upper  	= 56.0f;
float offset_upper 		= -74.0f;

float amplitude_lower 	= -50.5f;
float phaseShift_lower 	= -1.928f;
float offset_lower 		= 3.0f;
static void computeCPGGait() {
	phaseShift_upper 	= 0.0f;
	amplitude_upper  	= (ppData[0].max-ppData[0].min)/1024.0f;
	offset_upper 		= (ppData[0].max+ppData[0].min)/2.0f;

	amplitude_lower 	= (ppData[1].max-ppData[1].min)/1024.0f;
	phaseShift_lower 	= (ppData[1].max-ppData[1].min)/2.0f;
	offset_lower 		= (ppData[1].max-ppData[1].min)/2.0f;
}*/
static void logPosition() {
	ase_printf("#log pos.log {");
	int nAct=getNumberOfActuators();
	for(int i=0;i<nAct;i++) {
			int pos = dynamixelApi_getPosition(i);
			ase_printf("%i",pos);
			if(i!=nAct-1) ase_printf(", ");
	}
	ase_printf("},\n");
}

void logPos_start(void* data) {
	for(int i=0;i<getNumberOfActuators();i++) {
		dynamixelApi_setWheelMode(i, true);
		dynamixelApi_setTorqueLimit(i, 1023);
	}
}

void logPos_stop(void* data) {
	for(int i=0;i<getNumberOfActuators();i++) {
		dynamixelApi_setTorqueLimit(i, 1023);
	}
}
int oldPos[10];
int cPos[10];
void logPos_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	if(itt%4==0) {
		logPosition();
	}
	for(int i=0;i<nOutputs;i++) {
		output[i] = 127;
	}
	for(int i=0;i<getNumberOfActuators();i++) {
		cPos[i]=dynamixelApi_getPosition(i);
		int speed = (cPos[i]-oldPos[i]);
		oldPos[i] = cPos[i];
		if(speed>1){
			dynamixelApi_setMovingSpeed(i, 150);
		}
		else if(speed<-1) {
			dynamixelApi_setMovingSpeed(i, 1024+150);
		}
		else {
			dynamixelApi_setMovingSpeed(i, 0);
		}
	}
}

static int cpgGoalPos[25];
static void cpg_setActuatorPos(HCCpg_t* cpg, int actuator) {
	float goal = CpgSuper_getX((CpgSuper_t*)cpg);
	//ase_printf("{%i,%i},\n",actuator,(int)(100*goal));
	cpgGoalPos[actuator] = (int)goal;
}

void cpg_standUp() {
	CpgGait_setGlobalFrequency(0);
	CpgGait_setOffset(-3.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-3.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_BACK_UPPER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_BACK_LOWER_INDEX);
}

void cpg_stopGait() {
	CpgGait_setGlobalFrequency(0);
}


void cpg_forwardGait(float freq) {
	CpgGait_setGlobalFrequency(freq);

	CpgGait_setOffset(-3.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-3.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setOffset(-74.0f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setAmplitude(56.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setAmplitude(50.5f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setAmplitude(50.5f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setPhaseShift(0, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, LEFT_BACK_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setPhaseShift(-1.928f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_BACK_LOWER_INDEX);
}

void cpg_backwardGait(float freq) {
	CpgGait_setGlobalFrequency(freq);

	CpgGait_setOffset(-3.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-3.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setOffset(-74.0f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setAmplitude(-56.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setAmplitude(50.5f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setAmplitude(+50.5f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setPhaseShift(0, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, LEFT_BACK_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setPhaseShift(-1.928f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_BACK_LOWER_INDEX);
}
void cpg_turnLeftGait(float freq) {
	CpgGait_setGlobalFrequency(freq);

	CpgGait_setOffset(-3.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-3.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setOffset(-74.0f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setAmplitude(-56.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setAmplitude(50.5f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setAmplitude(50.5f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setPhaseShift(0, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, LEFT_BACK_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setPhaseShift(-1.928f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_BACK_LOWER_INDEX);
}
void cpg_turnRightGait(float freq) {
	/*for(int i=0;i<getNumberOfActuators();i++) {
		CpgGait_setAmplitude(amplitude, i);
		CpgGait_setOffset(0.0f, i);
		CpgGait_setFrequency(freq, i);
		if(i%2==0) CpgGait_setPhaseShift(3.14f, i);
		else CpgGait_setPhaseShift(0, i);
	}*/
	CpgGait_setGlobalFrequency(freq);

	CpgGait_setOffset(-3.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setOffset(-3.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setOffset(+3.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setOffset(-74.0f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setOffset(-74.0f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setAmplitude(56.0f, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setAmplitude(-56.0f, LEFT_BACK_UPPER_INDEX);
	CpgGait_setAmplitude(56.0f, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setAmplitude(50.5f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setAmplitude(-50.5f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setAmplitude(50.5f, RIGHT_BACK_LOWER_INDEX);

	CpgGait_setPhaseShift(0, LEFT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_FRONT_UPPER_INDEX);
	CpgGait_setPhaseShift(0, LEFT_BACK_UPPER_INDEX);
	CpgGait_setPhaseShift(0, RIGHT_BACK_UPPER_INDEX);

	CpgGait_setPhaseShift(-1.928f, LEFT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_FRONT_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, LEFT_BACK_LOWER_INDEX);
	CpgGait_setPhaseShift(-1.928f, RIGHT_BACK_LOWER_INDEX);
}

void cpg_start(void* data){
	CM510Behavior_cpg_t* cpg_data = (CM510Behavior_cpg_t*) data;
	if(cpg_data->initialized == 0) {
		CpgGait_init();
		CpgGait_setActuatorPosHandler(cpg_setActuatorPos);
		CpgGait_setUseFeedback(0);

		for(int i=0;i<getNumberOfActuators();i++) {
			CpgGait_setAmplitude(100.0f, i);
			CpgGait_setOffset(0.0f, i);
			CpgGait_setFrequency(0.5f, i);
			CpgGait_setPhaseShift(0, i);
		}
		CpgGait_start();
		cpg_data->initialized = 1;
	}
}

void logCPG(int nOutputs, signed char forward, signed char turn) {
	ase_printf("#log cpg.log {%li, ",getLocalMsTime());
	for(int i=0;i<nOutputs;i++) {
			ase_printf("%i",cpgGoalPos[i]);
			if(i!=nOutputs-1) ase_printf(", ");
	}
	ase_printf("},\n");
	ase_printf("#log ctrl.log {%i, %i},\n", forward, turn);
}


void cpg_stop(void* data) {}

void cpg_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	long time = LuiManager_getDeviceRCListUpdateTime();
	for(int i=0;i<nOutputs;i++) {
		/*if((getLocalMsTime()-time)<1000) output[i] = cpgGoalPos[i];
		else output[i] = 127;*/
		output[i] = cpgGoalPos[i];
	}
	signed char forward = LuiManager_getDeviceRCList()[0];
	signed char turn = LuiManager_getDeviceRCList()[1];
	if(itt%1==0) {
		logCPG(nOutputs, forward, turn);
		logPosition();
	}
	itt++;
	float maxHz = 0.1f;
	if((getLocalMsTime()-time)<1000)  {
		//ase_printf("RC = (%i, %i) \n",forward, turn);
		if(forward>10 && abs(turn)<=abs(forward)) { //forward
			float freq = fabs(maxHz*((float)forward)/100.0f);
			cpg_forwardGait(freq);
		}
		else if(forward<-10 && abs(turn)<=abs(forward)) { //backward
			float freq = fabs(maxHz*((float)forward)/100.0f);
			cpg_backwardGait(freq);
		}
		else if(turn>10 && abs(turn)>=abs(forward)) { //turn left
			float freq = fabs(maxHz*((float)turn)/100.0f);
			cpg_turnLeftGait(freq);
		}
		else if(turn<-10 && abs(turn)>=abs(forward)) { //turn right
			float freq = fabs(maxHz*((float)turn)/100.0f);
			cpg_turnRightGait(freq);
		}
		else { //stop
			cpg_stopGait();
		}
	}
	else {
		cpg_standUp();
	}
}

