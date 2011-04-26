#include <stdio.h>
#include <stdlib.h>

#include <ase/targets/AbstractModuleApi.h>
#include <ase/DynamixelApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/LearningCpgGait/SPSACpgGait/SPSACpgGait.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/tools/Timer/TimerManager.h>

static FILE *logReward;
Timer_t* sendRewardTimer;
Timer_t* updateRewardTimer;
static float reward;

static void act(char* topic, Event_t* event){}
static void handleMessage(char* topic, Event_t* event) {}

static void updateReward() {
	int i,j,p1,p2;
	int sumOfSquares = 0;
	for(i=0;i<getNumberOfActuators();i++) {
		p1 = dynamixelApi_getPosition(i);
		for(j=i+1;j<getNumberOfActuators();j++) {
			p2 = dynamixelApi_getPosition(j);
			sumOfSquares += (p1-p2)*(p1-p2);
		}
	}
	reward += sumOfSquares;
}

static void timerFired(int id) {
	SPSALearningMsg_t msg;
	msg.super.msgType = LEARNING_MESSAGE;
	msg.super.label = 0;
	msg.stateCount++;
	msg.reward = -reward;
	fprintf(logReward, "%f %f\n", getLocalTime(), reward);
	fflush(logReward);
	reward=0;
	ase_handleMessage((char*)&msg,sizeof(msg),INTERNAL_MESSAGE);
}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	reward=0;
	srand(getRandomSeed());
	sendRewardTimer = TimerManager_createPeriodicTimer(1000,0,timerFired);
	updateRewardTimer = TimerManager_createPeriodicTimer(100,0,updateReward);
	dynamixelApi_connect(1);
	dynamixelApi_connect(2);
	dynamixelApi_connect(3);
	dynamixelApi_connect(4);

	SPSACpgGait_init();
	int i;
	for(i=0;i<getNumberOfActuators();i++) {
		SPSACpgGait_setFrequencyInterval(1.0f, 1.0f, 1.0f);
		SPSACpgGait_setAmplitudeInterval(0.0f, 0.25f, 0.5f,i);
		SPSACpgGait_setPhaseshiftInterval(0.0f, 0.0f, 0.0f,i);
		SPSACpgGait_setOffsetInterval(0.5f, 0.5f, 0.5f ,i);
		SPSACpgGait_updateGait();
	}
	SPSACpgGait_setLearningParameters(0.05f, 0.05f);
	SPSACpgGait_reset(1);
	CpgGait_setCouplingStrenghts(0.5f, 0.5f, 0.5f);
	if(!CpgGait_isStarted()) {
		CpgGait_start();
	}

	logReward = fopen("data.txt", "w");
}
