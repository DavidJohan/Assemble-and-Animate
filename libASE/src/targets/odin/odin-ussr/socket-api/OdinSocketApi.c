#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/targets/odin-ussr.h>
#include <ase/utils/socket-tools/CommandSender.h>
#include <ase/utils/socket-tools/EventListner.h>
#include <ase/utils/socket-tools/SimpleSocket.h>

#define MAX_MSG_SIZE	50

char odinApi_msgReceiveBuffer[MAX_MSG_SIZE];
char odinApi_msgSendBuffer[MAX_MSG_SIZE];
void (*odinApi_msgHandler)(char*, char, char);

int odinApi_wasSend() {
	return cs_wasSuccess();
}

char* odinApi_getName(char* returnString) {
	return cs_sendCommand_string("getName",returnString);
}

int odinApi_isActuating() {
	return cs_sendCommand_bool("isActuating");
}

void odinApi_actuate(float pos) {
	char str[50]; sprintf(str, "actuate %f", pos);
	cs_sendCommand_void(str);
}

void odinApi_disactivate() {
	cs_sendCommand_void("disactivate");
}

float odinApi_getTime() {
	char str[50]; 
	sprintf_take();
	sprintf(str, "getTime");
	sprintf_release();
	return cs_sendCommand_float(str);
}

float odinApi_getActuatorPosition() {
	char str[50]; sprintf(str, "getActuatorPosition");
	return cs_sendCommand_float(str);
}

int odinApi_isConnected(int connector) {
	char str[50]; sprintf(str, "isConnected %d", connector);
	return cs_sendCommand_bool(str);
}

void odinApi_yield() {
	cs_sendCommand_void("yield");
}

void odinApi_delay(long ms) {
	char str[20]; 
	sprintf_take();
	sprintf(str, "delay %li", ms);
	sprintf_release();
	cs_sendCommand_void(str);
}

void charArrayToString(char* array, char* result, char arrayLength) {
	char charBuffer[10];
	int i,j,n,index = 0;
	for(i=0;i<arrayLength;i++) { 
		n=sprintf(charBuffer, "%d",array[i]);
		for(j=0;j<strlen(charBuffer);j++) {
			result[index++] = charBuffer[j];
		}
		result[index++] = ',';
	}
	result[index] = '\0';
}

void stringToCharArray(char* string, char* result) {
	int index=0;
	char* token;
	for ( token = strtok(string,",");token != NULL; token = strtok(NULL, ",") ) {
		result[index++] = atoi(token);
	}
	result[index] = '\0';
}

//returns int as boolean
char odinApi_messageStr[500];
int odinApi_sendMessage(char* message, char messageSize, char connector) {
	charArrayToString(message, odinApi_messageStr, messageSize);
	sprintf(odinApi_msgSendBuffer, "sendMessage %s %d %d", odinApi_messageStr, messageSize, connector);
	return cs_sendCommand_int(odinApi_msgSendBuffer);
}

void odinApi_handleMessage(char* eventType, char* messageData) {
	char* message = strtok(messageData," "); //handleMessage string
	int messageSize = atoi(strtok(NULL, " "));
	int channel = atoi(strtok(NULL, " "));
	stringToCharArray(message, odinApi_msgReceiveBuffer);
	odinApi_msgHandler(odinApi_msgReceiveBuffer, messageSize, channel);
}

void odinApi_setup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char)) {
	/*Initialize Message Handler*/
	odinApi_msgHandler = msgHandler;

	/*Initialize command sender*/
	cs_init(portRC, host);
	cs_sendCommand_void("setup"); //initializes simulator	
	
	/*Initialize event listener*/
	el_init(portEvent, host);
	el_installEvent("handleMessage", odinApi_handleMessage);
	el_startEventListen();
}
