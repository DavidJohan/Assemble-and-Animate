//TODO generalize these to abstract module module
/*void waitUntil(float endTime) {
	while(endTime>=atronApi_getTime());
}*/
#include <ase/targets/atron.h>

float getLocalTime() {
	return atronApi_getTime();
}

long getLocalMsTime() {
	return atronApi_getMsTime();
}

long getRandomSeed() {
	return atronApi_getRandomSeed();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	return atronApi_sendMessageToAll(msg, length);
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	return atronApi_sendMessageToAllExcept(msg, length, except);
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	return atronApi_sendMessage(msg, length, channel);
}

int getNumberOfNeighborChannels() {
	return 8;
}

int hasNeighbor(int channel) {
	if(channel>=0&&channel<8) {
		if(atronApi_isOtherConnectorNearby(channel))  {
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
	return atronApi_getHardwareID();
}

int setActuatorPosition(float position, int actuator) {
	if(actuator==0) {
		atronApi_rotateToDegree(6.283185f*position);
	}
	return 0;
}

float getActuatorPosition(int actuator) {
	if(actuator==0) {
		return atronApi_getAngularPosition()/6.283185f;
	}
	return -1;

}

int getNumberOfActuators() {
	return 1;
}



