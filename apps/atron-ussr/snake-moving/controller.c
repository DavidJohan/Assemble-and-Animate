#include <ase/targets/atron/AtronApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/CPGManager.h>
#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/communication/CommDefs.h>
#include <math.h>

#define UNKNOWN_ROLE	0
#define HEAD_OF_SNAKE 	1
#define TAIL_OF_SNAKE 	2
#define SPLINE_OF_SNAKE 3

#define N_MODULES 	7
#define PHASESHIFT 	6.28f/N_MODULES
#define AMPLITUDE	1.5
#define FREQUENCY	0.1

char moduleName[50];
int moduleRole = UNKNOWN_ROLE;
cpg_struct* cpgGlobal;
cpg_struct* cpgLocal;
cpgCoupling_struct*  coupling;
gradient_struct* headGradient;
	

int getNeighborCount(int startChannel, int endChannel) {
	int i, count = 0;
	for(i=startChannel;i<=endChannel;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			count++;
		}
	}
	return count;
}

int detectRole() {
	int southCount = getNeighborCount(0, 3);
	int northCount = getNeighborCount(4, 7);
	if(southCount==0&&northCount==1) {
		return HEAD_OF_SNAKE;
	}
	else if(southCount==1&&northCount==0) {
		return TAIL_OF_SNAKE;
	}
	else if(southCount==1&&northCount==1) {
		return SPLINE_OF_SNAKE;
	}
	else {
		return UNKNOWN_ROLE;
	}
}

int distFromHead() {
	return N_MODULES-gradient_getValue(headGradient);
}

void cpgControl() {
	if(CPGManager_act()>0) { }
	coupling->phaseDiff = distFromHead()*PHASESHIFT;
}

void gradientControl() {
	if(GradientManager_act()>0) { }
	if(detectRole()==HEAD_OF_SNAKE) {
		headGradient->seed = N_MODULES;
	}
	else {
		headGradient->seed = 0;
	}
}
		

void actuatorControl() {
	float goal = 3.1428f; //int?-mistake???
	if(detectRole()==SPLINE_OF_SNAKE) {
		if(distFromHead()%2==0) {
			goal += cpg_getX(cpgLocal);
		}
		else {
			goal -= cpg_getX(cpgLocal);
		}
	}
	atronApi_rotateToDegree(goal);
}

void controlLoop() {
	printf("%s: Started...\n", moduleName);
	while(1) {
		cpgControl();
		gradientControl();
		actuatorControl();
		atronApi_yield();
	}
}

void controller_start()  {
	atronApi_yield();
	atronApi_getName(moduleName);

	cpgGlobal = CPGManager_createCPG(1, FREQUENCY);
	cpgLocal = CPGManager_createCPG(2, FREQUENCY);
	cpgLocal->amplitude = AMPLITUDE;

	CPGManager_createCoupling(1, 1, 1, 0);
	coupling = CPGManager_createCoupling(1, 2, 1, 0);
	headGradient = GradientManager_createGradient(1, 2);
	
	controlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CPG_MESSAGE) {
		CPGManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==GRADIENT_MESSAGE) {
		GradientManager_handleMessage(message, messageSize, channel);	
	}
	else {
		printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
