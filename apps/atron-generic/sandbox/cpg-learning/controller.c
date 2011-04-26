#include <stdio.h>
//#include <stdint.h>
#include <stdlib.h>
//#include <stdbool.h>
//#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/CPGManager.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/control/strategies/Learning/LearningManager.h>

learning_struct* learningProcess;

void controller_init() {
	printf("%i: Running\n",getHardwareID());
	srand (getRandomSeed());
	CpgGait_init();
	learningProcess = LearningManager_createLearningProcess(1, 5.0f);
	//learningProcess->alpha = 0.05;
	//learningProcess->nActions = 2;
	CpgGait_setPhaseShift(0.0f); //2.464f
	CpgGait_setFrequency(0.20f);//0.15f
	CpgGait_setAmplitude(0.0f);
}

void bestKnownWalker1Gait() {
	int myID = getHardwareID();
	float ampl = 60.0f;
	float offset = 45.0f;
	CpgGait_setFrequency(0.15f);
	//legs
	if(myID==0) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setOffset(offset);
		CpgGait_setPhaseShift(2.0f*6.283185f/4.0f);
	}
	if(myID==1) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setOffset(+offset);
		CpgGait_setPhaseShift(3.0f*6.283185f/4.0f);
	}


	if(myID==2) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setOffset(-offset);
		CpgGait_setPhaseShift(1.0f*6.283185f/4.0f);
	}
	if(myID==3) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setOffset(-offset);
		CpgGait_setPhaseShift(0.0f*6.283185f/4.0f);
	}
	ampl=0;

	//feets
	if(myID==4) {  //|| myID==8 || myID==3 || myID==4)
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==5) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==6) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(2.0f*6.283185f/4.0f);
	}
	if(myID==7) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(2.0f*6.283185f/4.0f);
	}


}


void bestKnownWalker2Gait() {
	int myID = getHardwareID();
	float ampl = 60.0f;

	//feets
	if(myID==3) {  //|| myID==8 || myID==3 || myID==4)
		//CpgGait_setAmplitude(ampl);
		//CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==4) {
		//CpgGait_setAmplitude(ampl);
		//CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==7) {
		//CpgGait_setAmplitude(ampl);
		//CpgGait_setPhaseShift(0.0f);
	}
	if(myID==8) {
		//CpgGait_setAmplitude(ampl);
		//CpgGait_setPhaseShift(0.0f);
	}
	//spine
	if(myID==0) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(3.1415f);
	}
	//legs
	if(myID==1) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==2) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(3.1415f);
	}
	if(myID==5) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(0.0f);
	}
	if(myID==6) {
		CpgGait_setAmplitude(ampl);
		CpgGait_setPhaseShift(0.0f);
	}
}

float phaseShift;
int count = 0;
void controller_act()  {
	if(LearningManager_act()>0) {

		int accelerated = learning_accelerateNow(learningProcess);
		int action = learning_getAction(learningProcess);
		printf("%i: Updated at time = %f sec\n", getHardwareID(), getLocalTime());
		printf("    Action = %i, accelerated = %i\n", action, accelerated);
		if(action == 0) {
			CpgGait_setAmplitude(60.0f);
			CpgGait_setPhaseShift(0.0f);
		}
		if(action == 1)  {
			CpgGait_setAmplitude(60.0f);
			CpgGait_setPhaseShift(3.1415f);
		}
		if(action == 2) {
			CpgGait_setAmplitude(0);
		}
		bestKnownWalker1Gait();
	}
	CpgGait_act();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CPG_MESSAGE) {
		CPGManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==LEARNING_MESSAGE) {
		LearningManager_handleMessage(message, messageSize, channel);
	}
	else {
		printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
