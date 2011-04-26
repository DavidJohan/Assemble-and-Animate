#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/control/strategies/Learning/LearningManager.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/config/ConfigASE.h>


LearningMsg_t learningMsg;
Learning_t learningProcesses[LEARNING_PROCESSES_MAX];
LearningUpdate_t updates[LEARNING_PROCESSES_MAX];



int LearningManager_act() {
	float now = getLocalTime();
	int updateCounts=0, i;
	for(i=0;i<LEARNING_PROCESSES_MAX;i++) {
		if(learningProcesses[i].label!=0) { //label 0 indicate not used
			if(updates[i].nextUpdateTime<=now && learning_isReady(&learningProcesses[i])) { //if time to update and process is ready (got reward)
				learning_update(&learningProcesses[i]);
				updates[i].nextUpdateTime += updates[i].deltaTime;
				updateCounts++;
			}
		}
	}
	return updateCounts;
}

Learning_t* LearningManager_createLearningProcess(short label, float deltaTime) {
	int i;
	for(i=0;i<LEARNING_PROCESSES_MAX;i++) {
		if(learningProcesses[i].label==0) { //label 0 indicate not used
			learning_init(&learningProcesses[i]);
			learningProcesses[i].label = label;
			updates[i].deltaTime = deltaTime;
			updates[i].nextUpdateTime = getLocalTime() + updates[i].deltaTime;
			return &learningProcesses[i];
		}
	}
	#ifdef PRINTF
	ase_printf("Warning: #1 in LearningManager \n"); //  unable to create new learning process - not enourgh space allocated LEARNING_PROCESSES_MAX
	#endif
	return NULL;
}

void LearningManager_updateFromMsg(LearningMsg_t* learningMsg) {
	int i;
	for(i=0;i<LEARNING_PROCESSES_MAX;i++) {
		if(learningProcesses[i].label==learningMsg->super.label) {
			learning_collectReward(&learningProcesses[i], learningMsg->reward);
			//printf("Message: reward %f \n",learningMsg->reward);
		}
	}
}

static void handleMessage(Msg_t* msg) {
	if(msg->message[0] == LEARNING_MESSAGE) {
		LearningMsg_t* learningMsg = (LearningMsg_t*) msg->message;
		LearningManager_updateFromMsg(learningMsg);
	}
}

void LearningManager_init() {
	MsgManager_subscribe(LEARNING_MESSAGE, handleMessage);
}
