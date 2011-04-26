#include <ase/targets/roombot-webots.h>
#include <stdio.h>

float getLocalTime() {
	return roombotApi_getLocalTime();
}

long getLocalMsTime() {
	return roombotApi_getLocalMsTime();
}

long getRandomSeed() {
	return roombotApi_getRandomSeed();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	roombotApi_sendMessageToAll(msg, length);
	return 1;
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	ase_printf("Warning: sendMessageToAllExcept not implemented from Roombots!\n");
	return 0;
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	roombotApi_sendMessage(msg, length, channel);
	return 1;
}



int hasNeighbor(int channel) {
	if(channel>=0&&channel<10) {
		if(roombotApi_getConnectorPresence(channel))  {
			return 1;
		}
		else {
			return 0;
		}
	}
	else {
		return -1;//undefined
	}
}

int getHardwareID() {
	return roombotApi_getID();
}

int setActuatorPosition(float position, int actuator) {
	if(actuator>=0&&actuator<3) {
		float goal = 2*(position-0.5f)*3.14159f;
		roombotApi_setServoPosition(actuator, goal);
		return 1;
	}
	return 0;
}

float getActuatorPosition(int actuator) {
	if(actuator>=0&&actuator<3) {
		float pos = (roombotApi_getServoPosition(actuator)/3.14159f+1)/2;

		if(pos>1||pos<0) {
		//	printf("bug: return pos is %f",pos);
		}
		return pos;
	}
	return -1;
}

int getNumberOfActuators() {
	return 3;
}

int getNumberOfNeighborChannels() {
	return 10;
}
