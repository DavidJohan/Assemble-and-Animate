#include <ase/targets/atron/AtronApi.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/control/strategies/Learning/LearningManager.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ase/communication/CommDefs.h>

learning_struct* learningProcess;
char moduleName[50];
int rewardUpdated=0;
int reward;

void mainControlLoop() {
	while(1) {
		if(LearningManager_act()>0) {
			int accelerated = learning_accelerateNow(learningProcess);
			int action = learning_getAction(learningProcess);
			printf("Action = %i, accelerated = %i\n", action, accelerated);
			if(action == 0) atronApi_centerStop();
			if(action == 1) atronApi_rotateContinuous(1);
			if(action == 2) atronApi_rotateContinuous(-1);
		}
		atronApi_yield();
	}
}

int seedFromName() {
	int i, sum=0;
	for(i=0;i<strlen(moduleName);i++) {
		sum += moduleName[i];
	}
	return sum;
}

void controller_start()  {
	atronApi_getName(moduleName);
	printf("Controller started %i\n",seedFromName());
	srand ( seedFromName());
	learningProcess = LearningManager_createLearningProcess(1, 7.0f);
	
	mainControlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==LEARNING_MESSAGE) {
		LearningManager_handleMessage(message, messageSize, channel);
	}
}
