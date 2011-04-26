#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/control/behaviors/generic/LearningCpgGait/SPSACpgGait/SPSACpgGait.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/control/behaviors/generic/LearningCpgGait/LearningCpgGait.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <math.h>

enum { LEARNING, EXPLOIT, RANDOM, HOME };
static int state;
static SPSALearning_t learningProcess;
static float ak=0.001f, ck=0.1f;

static float getParameter(SPSALearning_t* process, int index, int wrap) {
	if(state==LEARNING) 	return SPSALearning_getTheta(process, index, wrap);
	else if(state==EXPLOIT) return SPSALearning_getThetaNonPerturbed(process, index);
	else if(state==RANDOM) 	return ((float)rand())/(((float)RAND_MAX)+0.1f);
	else if(state==HOME) {return -1.0;}
	return -1.0f;
}

static void initLearning() {
	SPSALearning_init(&learningProcess);
	learningProcess.ak = ak;
	learningProcess.ck = ck;
	SPSACpgGait_setStateLearning();
}

void SPSACpgGait_setLearningParameters(float ak_new, float ck_new) {
	ak = ak_new;
	ck = ck_new;
	learningProcess.ak = ak_new;
	learningProcess.ck = ck_new;
}
//TODO kommentarer helt forkert - hvad er hvad?
static float* getParameters(SPSALearning_t* process, float* result) {
	int i, j, index=0;
	for(i=0;i<getNumberOfActuators();i++) {
		result[index] = getParameter(process, index, 1); //phaseShift
		//result[index] = getParameter(process, index, 0); //phaseShift
		index++;

		result[index] = getParameter(process, index, 0); //amplitude
		index++;

		result[index] = getParameter(process, index, 0); //offset
		index++;

		for(j=0;j<CPGGAIT_N_FEEDBACK_TERMS;j++) {
			result[index] = getParameter(process, index, 0); //FeedbackTerm
			index++;
		}
	}
	result[index] = getParameter(process, index, 0); //frequency
	return result;
}
void SPSACpgGait_updateGait() {
	float params[learningProcess.nParameters];
	getParameters(&learningProcess, (float*)&params);
	LearningCpgGait_updateGait((float*)&params);
}

void SPSACpgGait_reset(int randInit)  {
	SPSALearning_reset(&learningProcess, randInit);
	SPSACpgGait_updateGait();
}

void SPSACpgGait_setThetaFromCpgGait()  {
	int nParams = getNumberOfActuators()*8+1;
	float theta[nParams];
	LearningCpgGait_getScaledController(theta);
	int i;
	ase_printf("Theta: ");
	for(i=0;i<nParams;i++) {
		ase_printf("%f, ",theta[i]);
	}
	ase_printf("\n");
	SPSALearning_setTheta(&learningProcess, theta);
	SPSACpgGait_updateGait();
}

void SPSACpgGait_setStateExploit() 	{state = EXPLOIT;}
void SPSACpgGait_setStateLearning() {state = LEARNING;}
void SPSACpgGait_setStateRandom() 	{state = RANDOM;}
void SPSACpgGait_setStateHome() 	{state = HOME;}


void SPSACpgGait_logLearningParameters(int id, FILE* log) {
	#ifdef FPRINTF
	if(log!=0) {
		fprintf(log,"%i %f %f\n",id, (double)ak, (double)ck);
		fflush(log);
	}
	#endif
}
void SPSACpgGait_logController(int id, FILE* log) {
	float params[learningProcess.nParameters];
	getParameters(&learningProcess, (float*)&params);
	LearningCpgGait_logController(id,log,params);
}
static void act(char* topic, Event_t* event)  {
	if(SPSALearning_isReady(&learningProcess)) {
		if(state==LEARNING) {
			SPSALearning_update(&learningProcess);
			LearningCpgGait_signalUpdated();
		}
		else {
			SPSALearning_ignoreUpdate(&learningProcess);
		}
		SPSACpgGait_updateGait();
	}
}

/**
 * \todo how to handle problems communicating floats?
 */
static void handleMessage(Msg_t* msg) {
	if(msg->message[0]==LEARNING_MESSAGE) {
		float reward;
		if(msg->messageSize==sizeof(SPSALearningMsg_t)) {
			SPSALearningMsg_t* spsaMsg = (SPSALearningMsg_t*) msg->message;
			reward = spsaMsg->reward;
		}
		else if(msg->messageSize==sizeof(SPSALearningMsgByte_t)) {
			SPSALearningMsgByte_t* spsaMsg = (SPSALearningMsgByte_t*) msg->message;
			reward = (float) spsaMsg->reward;
		}
		#ifdef PRINTF
		ase_printf("Got reward = %f\n",reward);
		#endif
		if(state == LEARNING) {
			SPSALearning_collectReward(&learningProcess, reward);
		}
	}
}

void SPSACpgGait_init() {
	CpgGait_init();
	initLearning();
	LearningGpgGait_init();
	SPSACpgGait_updateGait();
	MsgManager_subscribe(LEARNING_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	CpgGait_start();
}

