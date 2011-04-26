#include <ase/targets/atron/AtronApi.h>
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


void mywait(int ms) { 
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
	float startTime =  atronApi_getTime();

	
	if(seedFromName()==49) {
		consensus_setLocalValue(1, &consensus);
	}
	else {
		consensus_setLocalValue(0, &consensus);
	}

	while(1) {
		printf("{%f, %s, %f},\n", (atronApi_getTime()-startTime), moduleName, consensus_getConsensusValue(&consensus));		consensus_update(&consensus);
		mywait(100);
	}
}

void controller_start()  {
	atronApi_getName(moduleName);
	printf("Controller started %i\n",seedFromName());
	srand ( seedFromName());
	consensus_init(&consensus);
	mywait(5000);
	mainControlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CONSENSUS_MESSAGE) {
		consensusMsg_struct* consensusMsg = ((consensusMsg_struct*) message);
		consensus_handleMessage(&consensus, consensusMsg, channel);	
	}
}
