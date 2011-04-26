#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/targets/atron.h>
#include <ase/utils/socket-tools/CommandSender.h>
#include <ase/utils/socket-tools/EventListner.h>
#include <ase/utils/socket-tools/SimpleSocket.h>

#define MAX_MSG_SIZE	50

char atronApi_msgReceiveBuffer[MAX_MSG_SIZE];
char atronApi_msgSendBuffer[MAX_MSG_SIZE];
void (*atronApi_msgHandler)(char*, char, char);

int atronApi_wasSend() {
	return cs_wasSuccess();
}

void atronApi_home() {
	cs_sendCommand_void("home");
}
void atronApi_setLeds(int val) {
	char str[20]; sprintf(str, "setLeds %d", val);
	cs_sendCommand_void(str);
}

int atronApi_getLeds() {
	char str[50];
	sprintf_take();
	sprintf(str, "getLeds");
	sprintf_release();
	return cs_sendCommand_int(str);
}


char* atronApi_getName(char* returnString) {
	return cs_sendCommand_string("getName",returnString);
}

int atronApi_isRotating() {
	return cs_sendCommand_bool("isRotating");
}

int atronApi_getJointPosition() {
	char str[20]; sprintf(str, "getJointPosition");
	return cs_sendCommand_int(str);
}
void atronApi_rotate(int dir)  {
	char str[20]; sprintf(str, "rotate %d", dir);
	cs_sendCommand_void(str);
}
void atronApi_rotateDegrees(int degrees)  {
	char str[50]; sprintf(str, "rotateDegrees %d", degrees);
	cs_sendCommand_void(str);
}
void atronApi_rotateToDegreeInDegrees(int degrees)  {
	char str[50]; sprintf(str, "rotateToDegreeInDegrees %d", degrees);
	cs_sendCommand_void(str);
}
void atronApi_rotateToDegree(float rad) {
	char str[50]; sprintf(str, "rotateToDegree %f", rad);
	cs_sendCommand_void(str);
}
void atronApi_rotateToViaInDegrees(int goalDegree, int viaDegree) {
	char str[50]; sprintf(str, "rotateToViaInDegrees %d %d", goalDegree, viaDegree);
	cs_sendCommand_void(str);
}
void atronApi_rotateDirToInDegrees(int to, int dir) {
	if(dir==0) dir = -1;
	char str[50]; sprintf(str, "rotateDirToInDegrees %d %d", to, dir);
	cs_sendCommand_void(str);
}

float atronApi_getTime() {
	char str[50];
	sprintf_take();
	sprintf(str, "getTime");
	sprintf_release();
	return cs_sendCommand_float(str);
}

long atronApi_getMsTime() {
	float time = atronApi_getTime();
	return (long)(time*1000.0);
}

long atronApi_getRandomSeed() {
	char str[50]; sprintf(str, "getRandomSeed");
	return (long) cs_sendCommand_int(str);
}


float atronApi_getAngularPosition() {
	char str[50]; sprintf(str, "getAngularPosition");
	return cs_sendCommand_float(str);
}
int atronApi_getAngularPositionDegrees() {
	char str[50]; sprintf(str, "getAngularPositionDegrees");
	return cs_sendCommand_int(str);
}
void atronApi_disconnectAll() {
	cs_sendCommand_void("disconnectAll");
}
void atronApi_connectAll()  {
	cs_sendCommand_void("connectAll");
}

int atronApi_canConnect(int connector) {
	char str[50]; sprintf(str, "canConnect %d", connector);
	return cs_sendCommand_bool(str);
}

int atronApi_canDisconnect(int connector) {
	char str[50]; sprintf(str, "canDisconnect %d", connector);
	return cs_sendCommand_bool(str);
}

int atronApi_isMale(int connector) {
	char str[50]; sprintf(str, "isMale %d", connector);
	return cs_sendCommand_bool(str);
}

void atronApi_connect(int connector)  {
	char str[50]; sprintf(str, "connect %d", connector);
	cs_sendCommand_void(str);
}

void atronApi_disconnect(int connector)  {
	char str[50]; sprintf(str, "disconnect %d", connector);
	cs_sendCommand_void(str);
}

int atronApi_isConnected(int connector) {
	char str[50]; sprintf(str, "isConnected %d", connector);
	return cs_sendCommand_bool(str);
}

//returns int as boolean
int atronApi_isDisconnected(int connector) {
	char str[50]; sprintf(str, "isDisconnected %d", connector);
	return cs_sendCommand_bool(str);
}
void atronApi_rotateContinuous(float dir) {
	char str[50]; sprintf(str, "rotateContinuous %f", dir);
	cs_sendCommand_void(str);
}

void atronApi_centerBrake() {
	cs_sendCommand_void("centerBrake");
}
void atronApi_centerStop() {
	cs_sendCommand_void("centerStop");
}

int atronApi_getBatteryLevel() { //dummy
	//TODO implement in USSR
	return 0;
}

//returns int as boolean
int atronApi_isOtherConnectorNearby(int connector) {
	char str[50];
	sprintf_take();
	sprintf(str, "isOtherConnectorNearby %d", connector);
	sprintf_release();
	return cs_sendCommand_bool(str);
}

//returns int as boolean
int atronApi_isObjectNearby(int connector) {
	char str[50]; sprintf(str, "isObjectNearby %d", connector);
	return cs_sendCommand_bool(str);
}

int atronApi_getHardwareID() {
	char str[50]; sprintf(str, "getModuleID");
	return cs_sendCommand_int(str);
}


//short used to be byte
short atronApi_getTiltX() {
	return cs_sendCommand_int("getTiltX");
}

//short used to be byte
short atronApi_getTiltY() {
	return cs_sendCommand_int("getTiltY");
}
//short used to be byte
short atronApi_getTiltZ() {
	return cs_sendCommand_int("getTiltZ");
}

void atronApi_yield() {
	cs_sendCommand_void("yield");
}

void atronApi_delay(long ms) {
	char str[20];
	sprintf_take();
	sprintf(str, "delay %li", ms);
	sprintf_release();
	cs_sendCommand_void(str);
	/*float endTime = atronApi_getTime()+ms/1000.0f;
	while(endTime>=atronApi_getTime());*/
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
char atronApi_messageStr[500];
int useAck = 0;
int atronApi_sendMessage(char* message, char messageSize, char connector) {
	charArrayToString(message, atronApi_messageStr, messageSize);
	sprintf(atronApi_msgSendBuffer, "sendMessage %s %d %d", atronApi_messageStr, messageSize, connector);
	if(connector>=0&&connector<8) {
		if(useAck) {
			if(atronApi_isOtherConnectorNearby(connector))  {
				return cs_sendCommand_int(atronApi_msgSendBuffer);
			}
			return 0;
		}
		else {
			cs_sendCommand_int(atronApi_msgSendBuffer); //send into blue air
			return 1;
		}
	}
	return 0;
}

int atronApi_sendMessageToAll(char* message, char messageSize) {
	int i, count=0;
	for(i=0;i<8;i++) {
		count += atronApi_sendMessage(message, messageSize, i);
	}
	return count;
}

int atronApi_sendMessageToAllExcept(char* message, char messageSize, char except) {
	int i, count=0;
	for(i=0;i<8;i++) {
		if(i!=except) {
			count += atronApi_sendMessage(message, messageSize, i);
		}
	}
	return count;
}




void atronApi_handleMessage(char* eventType, char* messageData) {
	char* message = strtok(messageData," "); //handleMessage string
	int messageSize = atoi(strtok(NULL, " "));
	int channel = atoi(strtok(NULL, " "));
	stringToCharArray(message, atronApi_msgReceiveBuffer);
	atronApi_msgHandler(atronApi_msgReceiveBuffer, messageSize, channel);
}
void atronApi_setup() {
}
void atronApi_ussrSetup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char)) {
	/*Initialize Message Handler*/
	atronApi_msgHandler = msgHandler;

	/*Initialize command sender*/
	cs_init(portRC, host);
	cs_sendCommand_void("setup"); //initializes simulator

	/*Initialize event listener*/
	el_init(portEvent, host);
	el_installEvent("handleMessage", atronApi_handleMessage);
	el_startEventListen();
}
