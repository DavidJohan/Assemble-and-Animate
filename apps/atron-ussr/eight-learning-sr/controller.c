#include <ase/AtronSocketApi.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/control/strategies/Learning/LearningManager.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ase/communication/CommDefs.h>

#define LEARNING_STATE	0
#define SR_TO_EIGHT	1
#define SR_TO_WALKER	2

learning_struct* learningProcess;
char moduleName[50];
int rewardUpdated=0;
int reward;
int state = SR_TO_EIGHT;

int countConnections() {
	int count=0;
	int i;
	for(i=0;i<8;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			count++;
		}
	}
	return count;
}

void wait(int ms) { 
	float endTime = atronApi_getTime()+ms/1000.0f;
	while(endTime>atronApi_getTime()) { atronApi_yield(); }
}


void learnToMove() {
	if(LearningManager_act()>0) {
			int accelerated = learning_accelerateNow(learningProcess);
			int action = learning_getAction(learningProcess);
			printf("Action = %i, accelerated = %i\n", action, accelerated);
			if(action == 0) atronApi_centerStop();
			if(action == 1) atronApi_rotateContinuous(1);
			if(action == 2) atronApi_rotateContinuous(-1);
	}
}

int getConnectedHash() {
	int hash=0;
	int i;
	for(i=0;i<7;i++) {
		if(atronApi_isConnected(i)) {
			hash = hash|(1<<i);
		}
	}
	return hash;
}

void SR2Eight() {
//	printf("%s: %i\n",moduleName,getConnectedHash());
	/*atronApi_home();
	wait(2000);
	atronApi_connectAll();
	wait(2000);*/
	
	
	/*if(countConnections()==4) {
		int i;
		for(i=0;i<4;i++) {
			if(atronApi_isConnected(i)&&atronApi_isMale(i)) {
				atronApi_disconnect(i);
			}
		}
	}*/
	//atronApi_disconnectAll();
	/*atronApi_home() 
	atronApi_isRotating()
	atronApi_disconnectAll() 
	atronApi_canConnect(int connector);
	atronApi_canDisconnect(int connector);
	atronApi_isConnected(int connector);*/

}

void SR2Walker() {
	
}

void mainControlLoop() {
	while(1) {
		if(state==LEARNING_STATE)  {
			learnToMove();
		}		
		if(state==SR_TO_EIGHT) {
			SR2Eight();
		}
		if(state==SR_TO_WALKER) {
			SR2Walker();
		}
		broadcaster_act();
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
	broadcaster_init();
	learningProcess = LearningManager_createLearningProcess(1, 7.0f);
	
	mainControlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==LEARNING_MESSAGE) {
		int seenBefore = broadcaster_broadcast(message, messageSize, channel);
		if(!seenBefore) {
			printf("%s: Got a learning msg \n", moduleName);
			LearningManager_handleMessage(message, messageSize, channel);
		}
	}
}
