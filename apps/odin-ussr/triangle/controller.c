#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ase/OdinSocketApi.h>

char moduleName[50];

void controller_start()  {
	odinApi_getName(moduleName);
	while(1) {
		printf("%s: Odin Module Running at time %f...\n", moduleName, odinApi_getTime());	
		odinApi_delay(10000);
		odinApi_actuate(0.0f);
		odinApi_delay(10000);
		odinApi_actuate(1.0f);
		odinApi_sendMessage("123",3,0);
	}
}

void handleMessage(char* message, char messageSize, char channel) {
	printf("%s: I got a message: %s\n", moduleName, message);
}
