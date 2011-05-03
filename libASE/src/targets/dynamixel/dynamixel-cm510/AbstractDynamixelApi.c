#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel/DynamixelApi.h>

float getLocalTime() {
	return dynamixelApi_getTime();
}

long getLocalMsTime() {
	return dynamixelApi_getMsTime();
}

long getRandomSeed() {
	return dynamixelApi_getRandomSeed();
}

int sendMessageToAllNeighbors(char* msg, int length) {
	return dynamixelApi_sendMessageToAll(msg, length);
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	return dynamixelApi_sendMessageToAllExcept(msg, length, except);
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	return dynamixelApi_sendMessage(msg, length, channel);
}

int getNumberOfNeighborChannels() {
	return 0;
}

int getHardwareID() {
	return dynamixelApi_getHardwareID();
}

int setActuatorPosition(float position, int actuator) {
	dynamixelApi_setGoalPos(actuator, (int)(1024*position));
	return 0;
}

float getActuatorPosition(int actuator) {
	int pos = dynamixelApi_getPosition(actuator);
	return ((float)pos)/1024.0f;
}

int getNumberOfActuators() {
	return dynamixelApi_countActuators();
}


