/*
 * AbstractFableApi.c
 *
 *  Created on: 09/nov/2012
 *      Author: walteruzz
 */

#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel/DynamixelApi.h>

#include "ase/targets/fable/firmware/ext_uart.h"

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
	//TODO:check everythin is initialized!

	for(int i=0;i<4;i++) {
		sendMessageToNeighbor(msg,length,ext_uart_channels[i]);
	}
	return 0;
}

int sendMessageToAllExcept(char* msg, char length, char except) {
	for(int i=0;i<4;i++) {
			if(ext_uart_channels[i]!=except)
				sendMessageToNeighbor(msg,length,ext_uart_channels[i]);
		}
	return 0;
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	ext_uart_send(channel, msg,length);
	return 0;
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



