#include <ase/targets/atron/AtronApi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/CPGManager.h>
#include <ase/communication/CommDefs.h>

char nameString[50];

// \todo starvation has been observed in a 6 module snake
void controller_start()  {
	atronApi_yield();
	atronApi_getName(nameString);
	cpg_struct* cpg1 = CPGManager_createCPG(1, 0.2);
	cpg_struct* cpg2 = CPGManager_createCPG(2, 0.2);
	CPGManager_createCoupling(1, 1, 1, 0);
	CPGManager_createCoupling(2, 2, 1, 0);
	CPGManager_createCoupling(1, 2, 1, 1.5707f);
	CPGManager_createCoupling(2, 1, 1, 6.283f-1.5707f);

	while(1) {
		if(CPGManager_act()>0) {
			printf("{%s, %f, %f, %f},\n", nameString,atronApi_getTime(), cpg_getX(cpg1), cpg_getX(cpg2));
		}
	}
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==CPG_MESSAGE) {
		CPGManager_handleMessage(message, messageSize, channel);
	}
}
