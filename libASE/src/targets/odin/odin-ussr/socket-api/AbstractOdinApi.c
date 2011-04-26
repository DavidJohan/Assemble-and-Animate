#include <stdio.h>
#include <ase/targets/odin-ussr.h>

float getLocalTime() {
	return odinApi_getTime();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	/*int i, msgCount=0;
	for(i=0;i<8;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			atronApi_sendMessage(msg, length, i);
			msgCount++;
		}
	}
	return msgCount;*/
	printf("Warning: not implemented yet...\n");
	return 0;
}
