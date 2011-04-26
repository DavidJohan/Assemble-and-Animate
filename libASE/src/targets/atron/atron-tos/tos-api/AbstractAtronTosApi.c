#include <ase/targets/atron.h>

float getLocalTime() {
	return atronApi_getTime();
}

long getLocalMsTime() {
	return atronApi_getMsTime();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	 //change to only send of channels with neighbors??
	 return atronApi_sendMessageToAll(msg, length);
}

long getRandomSeed() {
	return atronApi_getRandomSeed();
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	if(channel>=0&&channel<8) {
		//if(atronApi_isOtherConnectorNearby(channel))
		{
			return atronApi_sendMessage(msg, length, channel);
		}
	}
	return 0;
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	return atronApi_sendMessageToAllExcept(msg, length, except);
}

int getNumberOfNeighborChannels() {
	return 8;
}

int hasNeighbor(int channel) {
	/*if(channel>=0&&channel<8) {
		if(atronApi_isOtherConnectorNearby(channel))  {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return 2;//undefined
	}*/
	return 0;
}

int getHardwareID() {
	return atronApi_getHardwareID();
}
