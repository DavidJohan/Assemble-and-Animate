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
#include "SnakeBehavior.h"

static int cpgGoalPos[25];
static void cpg_setActuatorPos(HCCpg_t* cpg, int actuator) {
	float goal = CpgSuper_getX((CpgSuper_t*)cpg);
	//ase_printf("{%i,%i},\n",actuator,(int)(100*goal));
	cpgGoalPos[actuator] = (int)goal;
}

void snake_stopGait() {
	CpgGait_setGlobalFrequency(0);
}


void snake_forwardGait(float freq) {
	CpgGait_setGlobalFrequency(freq);
	for(int i=0;i<getNumberOfActuators();i++)  {
		CpgGait_setOffset(0, i);
		if(i%2==0) {
			CpgGait_setAmplitude(25.0f, i);
			CpgGait_setPhaseShift(2*3.142f*(i+1)/8.0f+(3.142f/1.0f), i); //0= bad, 1= very good, 2= good,
		}
		else {
			CpgGait_setAmplitude(65.0f, i);
			CpgGait_setPhaseShift(2*3.142f*i/8.0f, i);
		}
	}
}

void snake_backwardGait(float freq) {
	CpgGait_setGlobalFrequency(freq);
	for(int i=0;i<getNumberOfActuators();i++)  {
		CpgGait_setOffset(0, i);
		if(i%2==0) {
			CpgGait_setAmplitude(0.0f, i);
			CpgGait_setPhaseShift(0, i);
		}
		else {
			CpgGait_setAmplitude(65.0f, i);
			CpgGait_setPhaseShift(2*3.142f*i/8.0f, i);
		}
	}
}

void snake_turnLeftGait(float freq) {
	CpgGait_setGlobalFrequency(freq);
	for(int i=0;i<getNumberOfActuators();i++)  {
		if(i%2==0) {
			CpgGait_setOffset(0, i);
			CpgGait_setAmplitude(15.0f, i);
			CpgGait_setPhaseShift(2*3.142f*(i+1)/8.0f+(3.142f/2.0f), i); //0= turns right, 2=turns left
		}
		else {
			if(i<4) CpgGait_setOffset(-50, i);
			else CpgGait_setOffset(50, i);
			CpgGait_setAmplitude(45.0f, i);
			CpgGait_setPhaseShift(2*3.142f*i/8.0f, i);
		}
	}
}
/*Best turning gait
 * CpgGait_setGlobalFrequency(freq);
	for(int i=0;i<getNumberOfActuators();i++)  {
		if(i%2==0) {
			CpgGait_setOffset(0, i);
			CpgGait_setAmplitude(15.0f, i);
			CpgGait_setPhaseShift(2*3.142f*(i+1)/8.0f+(3.142f/2.0f), i); //0= turns right, 2=turns left
		}
		else {
			CpgGait_setOffset(-50, i);
			CpgGait_setAmplitude(45.0f, i);
			CpgGait_setPhaseShift(2*3.142f*i/8.0f, i);
		}
	}
	*/
void snake_turnRightGait(float freq) {
	CpgGait_setGlobalFrequency(freq);
	for(int i=0;i<getNumberOfActuators();i++)  {
		if(i%2==0) {
			CpgGait_setOffset(0, i);
			CpgGait_setAmplitude(15.0f, i);
			CpgGait_setPhaseShift(2*3.142f*(i+1)/8.0f+3.0f*(3.142f/2.0f), i); //0= bad, 1= very good, 2= good,
		}
		else {
			//CpgGait_setOffset(-50, i);
			if(i<4) CpgGait_setOffset(50, i);
			else CpgGait_setOffset(-50, i);
			CpgGait_setAmplitude(45.0f, i);
			CpgGait_setPhaseShift(2*3.142f*i/8.0f, i);
		}
	}
}

void snake_start(void* data){
	CM510Behavior_snake_t* cpg_data = (CM510Behavior_snake_t*) data;
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

void snake_stop(void* data) {}

void snake_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	long time = LuiManager_getDeviceRCListUpdateTime();
	for(int i=0;i<nOutputs;i++) {
		output[i] = cpgGoalPos[i];
	}
	signed char forward = LuiManager_getDeviceRCList()[0];
	signed char turn = LuiManager_getDeviceRCList()[1];
	float maxHz = 1.0f;
	if((getLocalMsTime()-time)<1000)  {
		if(forward>10 && abs(turn)<=abs(forward)) { //forward
			float freq = fabs(maxHz*((float)forward)/100.0f);
			snake_forwardGait(freq);
		}
		else if(forward<-10 && abs(turn)<=abs(forward)) { //backward
			float freq = fabs(maxHz*((float)forward)/100.0f);
			snake_backwardGait(freq);
		}
		else if(turn>10 && abs(turn)>=abs(forward)) { //turn left
			float freq = fabs(maxHz*((float)turn)/100.0f);
			snake_turnLeftGait(freq);
		}
		else if(turn<-10 && abs(turn)>=abs(forward)) { //turn right
			float freq = fabs(maxHz*((float)turn)/100.0f);
			snake_turnRightGait(freq);
		}
		else { //stop
			snake_stopGait();
		}
	}
	else {
		snake_stopGait();
	}
}

