#include <ase/AtronSocketApi.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/control/strategies/Consensus/ConsensusAverage.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#include <ase/communication/CommDefs.h>


consensus_struct consensus;
char moduleName[50];


void wait(int ms) { 
	float endTime = atronApi_getTime()+ms/1000.0f;
	while(endTime>atronApi_getTime()) { atronApi_yield(); }
}

int seedFromName() {
	int i, sum=0;
	for(i=0;i<strlen(moduleName);i++) {
		sum += moduleName[i];
	}
	return sum;
}


void mainControlLoop() {
	int id=seedFromName()-49;
	float startTime =  atronApi_getTime();
	int myCounter = 0;
	while(1) {
		float variance = consensus_getLocalAgreement(&consensus);
		float counter = consensus_getConsensusValue(&consensus);
		if(id == 0) {
			if(variance<0.0001 && fabs(counter-myCounter/8)<0.001) {
				myCounter+=8;
				consensus_setLocalValue(myCounter, &consensus);
			}
		}
		else {
			consensus_setLocalValue(0, &consensus);
		}
		printf("%f\t%s\t%f\t%f\n", (atronApi_getTime()-startTime), moduleName, consensus_getConsensusValue(&consensus),consensus_getLocalAgreement(&consensus));
		consensus_update(&consensus);
		wait(100);
	}
}

void controller_start()  {
	atronApi_getName(moduleName);
	printf("Controller started %i\n",seedFromName());
	srand ( seedFromName());
	consensus_init(&consensus);
	wait(5000);
	mainControlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CONSENSUS_MESSAGE) {
		consensusMsg_struct* consensusMsg = ((consensusMsg_struct*) message);
		consensus_handleMessage(&consensus, consensusMsg, channel);	
	}
}
