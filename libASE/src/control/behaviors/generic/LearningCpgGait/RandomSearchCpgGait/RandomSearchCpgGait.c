#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/control/behaviors/generic/LearningCpgGait/RandomSearchCpgGait/RandomSearchCpgGait.h>
#include <ase/control/strategies/RandomSearch/RandomSearch.h>
#include <ase/control/behaviors/generic/LearningCpgGait/LearningCpgGait.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <math.h>

enum { LEARNING, EXPLOIT, HOME };
static int state;
static RandomSearch_t learningProcess;

static float getParameter(RandomSearch_t* process, int index) {
	if(state==LEARNING) 	return RandomSearch_getTheta(process, index);
	else if(state==EXPLOIT) return RandomSearch_getMaxTheta(process, index);
	else if(state==HOME) {return 0.5f;} //TODO this is wrong
	return -1.0f;
}

static void initLearning() {
	RandomSearch_init(&learningProcess);
	RandomSearchCpgGait_setStateLearning();
}

void RandomSearchCpgGait_updateGait() {
	float params[learningProcess.nParameters];
	int i;
	for(i=0;i<learningProcess.nParameters;i++) {
		params[i] = getParameter(&learningProcess, i);
	}
	LearningCpgGait_updateGait(params);
}

void RandomSearchCpgGait_logCurrentController(int id, FILE* log) {
	float params[learningProcess.nParameters];
	int i;
	for(i=0;i<learningProcess.nParameters;i++) {
		params[i] = getParameter(&learningProcess, i);
	}
	LearningCpgGait_logController(id,log,params);
}

void RandomSearchCpgGait_logBestControlInfo(int id, FILE* log) {
	#ifdef FPRINTF
	fprintf(log,"%i ",id);
	fprintf(log,"%i ",RandomSearch_getMaxIndex(&learningProcess));
	fprintf(log,"%f ",RandomSearch_getMaxReward(&learningProcess));
	fprintf(log,"\n");
	fflush(log);
	#endif
}


void RandomSearchCpgGait_reset()  {
	RandomSearch_reset(&learningProcess);
	RandomSearchCpgGait_updateGait();
}

void RandomSearchCpgGait_setStateExploit() 	{state = EXPLOIT;}
void RandomSearchCpgGait_setStateLearning() {state = LEARNING;}
void RandomSearchCpgGait_setStateHome() 	{state = HOME;}

static void act(char* topic, Event_t* event)  {
	if(RandomSearch_isReady(&learningProcess)) {
		if(state==LEARNING) {
			RandomSearch_update(&learningProcess);
			LearningCpgGait_signalUpdated();
		}
		else {
			RandomSearch_ignoreUpdate(&learningProcess);
		}
		RandomSearchCpgGait_updateGait();
	}
}

/**
 * \todo how to handle problems communicating floats?
 */
static void handleMessage(Msg_t* msg) {
	if(msg->message[0]==LEARNING_MESSAGE) {
		float reward;
		if(msg->messageSize==sizeof(RandomSearchMsg_t)) {
			RandomSearchMsg_t* rndSearchMsg = (RandomSearchMsg_t*) msg->message;
			reward = rndSearchMsg->reward;
		}
		#ifdef PRINTF
		ase_printf("Got reward = %f (current max = %f)\n",reward, RandomSearch_getMaxReward(&learningProcess));
		#endif
		RandomSearch_collectReward(&learningProcess, reward);
	}
}

void RandomSearchCpgGait_init() {
	CpgGait_init();
	initLearning();
	LearningGpgGait_init();
	RandomSearchCpgGait_updateGait();
	MsgManager_subscribe(LEARNING_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	CpgGait_start();
}

