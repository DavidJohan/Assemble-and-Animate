//TODO generalize these to abstract module module
//COPY-PASTE from ATRON
/*void waitUntil(float endTime) {
	while(endTime>=atronApi_getTime());
}*/
#include <ase/targets/mtran.h>

float getLocalTime() {
	return mtranApi_getTime();
}

long getLocalMsTime() {
	return mtranApi_getMsTime();
}

long getRandomSeed() {
	return mtranApi_getRandomSeed();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	return mtranApi_sendMessageToAll(msg, length);
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	return mtranApi_sendMessageToAllExcept(msg, length, except);
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	return mtranApi_sendMessage(msg, length, channel);
}

int getNumberOfNeighborChannels() {
	return 6;
}

int hasNeighbor(int channel) {
	if(channel>=0&&channel<6) {
		if(mtranApi_isOtherConnectorNearby(channel))  {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 2;//undefined
	}
}

int getHardwareID() {
	return mtranApi_getHardwareID();
}


