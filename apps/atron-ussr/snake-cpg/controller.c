#include <ase/targets/atron/AtronApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/communication/CommDefs.h>

cpg_struct cpg;
cpgCoupling_struct coupling;
cpgMsg_struct cpgMsg;
char nameString[50];

void waitUntil(float endTime) {
	while(endTime>=atronApi_getTime());
}

void sendMessageToAll(char* msg, int length) {
	int i;
	for(i=0;i<8;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			atronApi_sendMessage(msg, length, i);
		}
	}
}

void sendState(cpg_struct *cpg) {
	cpgMsg.msgType = CPG_MESSAGE;
	cpgMsg.from_label = cpg->label;
	cpgMsg.x = cpg->x;
	cpgMsg.v = cpg->v;
	sendMessageToAll((char*)&cpgMsg, sizeof(cpgMsg));
}

// \todo starvation has been observed in a 6 module snake
void controller_start()  {
	atronApi_yield();
	atronApi_getName(nameString);
	//testDelay();
	cpg_init(&cpg);
	cpg.label = 1;
	coupling.to_label = 1;
	coupling.strength = 1;
	coupling.phaseDiff = 0;
	cpg_setUpdateRate(50, &cpg);
	float endTime = atronApi_getTime();
	while(1) {
		endTime += 0.1f;
		waitUntil(endTime);
		cpg_iterate(&cpg);
		sendState(&cpg);
		printf("{%s, %f, %f, %f}, \n", nameString, atronApi_getTime(), cpg.x, cpg.v);
		float angle = 0.5*3.1428f*cpg_getX(&cpg)+3.1428f;
		//printf("%s: Rotating to %f\n",nameString,angle);
		atronApi_rotateToDegree(angle);
		//printf("%s: %f\n", nameString,atronApi_getTime()-endTime);
	}
}

// \todo Calling a api function will cause a deadlock and breakdown of communication
void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CPG_MESSAGE) {
		cpgMsg_struct* cpgMsg = ((cpgMsg_struct*) message);
		if(cpgMsg->from_label==coupling.to_label) {
			float x = cpgMsg->x;
			float v = cpgMsg->v;
			cpg_addCoupling(x, v, &coupling, &cpg);
		}
	}
}
