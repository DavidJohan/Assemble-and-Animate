#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ase/utils/socket-tools/SimpleSocket.h>
#include <ase/utils/socket-tools/CommandSender.h>

#define BUFFERSIZE	100
static char cs_commandBuffer[BUFFERSIZE];
static char cs_receiveBuffer[BUFFERSIZE];
static int cs_sockfd;
static int cs_commandCounter;
static int cs_commandBufferIndex;
static int cs_success;
static int cs_busy;
static char cs_host[20];
static int cs_port;
static int cs_sendCmdCount = 1;
void cs_init(int port, char* host) {
	cs_port = port;
	strcpy(cs_host, host);
	cs_sockfd = ss_connect(cs_host, cs_port);
	cs_commandCounter=0;
	cs_commandBufferIndex=0;
	cs_success = 1;
	cs_busy = 0;
}
void cs_sendCommandCount(int sendCommandCount) {
	cs_sendCmdCount = sendCommandCount;
}
void cs_reconnect() {
	printf("Resetting connection\n");
	ss_close(cs_sockfd);
	cs_init(cs_port, cs_host);
}

static char debugBuffer[BUFFERSIZE];
int parseReceiveBuffer(char* returnParam) {
	memcpy(debugBuffer,cs_receiveBuffer,strlen(cs_receiveBuffer)+1);
	int tokenCounter = 0;
	char* token;
	char* p;
	for ( token = (char*)strtok_r(cs_receiveBuffer," \n",&p);token != NULL; token = (char*)strtok_r(NULL, " \n",&p) ) {
		if(tokenCounter==0) {
			if(atoi(token)!=cs_commandCounter) {
				printf("ERROR: Counter error in CommandSender since %i!=%i\n", atoi(token), cs_commandCounter);
				return 0;
			}
		}
		if(tokenCounter==1) {
			if(strcmp(token,"ERROR")==0) {
				printf("ERROR: Ack error in CommandSender: %s \n",debugBuffer);
				return 0;
			}
		}
		if(tokenCounter==2) {
			//printf("Return parameter: %s",token);
			memcpy(returnParam, token, strlen(token));
		}
		tokenCounter++;
	}
	return 1;
}

int cs_sendCommand(char* command, char* returnParam) {
	// \todo memory correption here
	memset(returnParam, 0, BUFFERSIZE);
	sprintf_take();
	if(cs_sendCmdCount) {
		sprintf(cs_commandBuffer, "%i %s\r\n", cs_commandCounter, command);
	}
	else {
		sprintf(cs_commandBuffer, "%s\r\n", command);
	}
	sprintf_release();
	//printf("cs_send: %s command = %s\n", cs_commandBuffer,command);
	ss_send(cs_commandBuffer, strlen(cs_commandBuffer), cs_sockfd);
	memset(cs_receiveBuffer, 0, BUFFERSIZE);
	ss_receive(cs_receiveBuffer, cs_sockfd);
	int cs_success;
	if(cs_sendCmdCount) {
		cs_success = parseReceiveBuffer(returnParam);
	}
	else {
		memcpy(returnParam,cs_receiveBuffer,strlen(cs_receiveBuffer)+1);
		cs_success = 1;
	}
	if(cs_success) {
		cs_commandCounter++;
		return 1;
	}
	else {
		return 0;
	}
}

static char returnBuffer[BUFFERSIZE];
void cs_sendCommand_void(char* command) {
	//printf("cs_send_void: command = %s\n", command);
	cs_success = cs_sendCommand(command,returnBuffer);
	if(!cs_success) printf("ERROR command: %s was not send or received correctly\n",command);
}

int cs_sendCommand_int(char* command) {
	int param;
	//printf("cs_send_int: command = %s\n", command);
	cs_success = cs_sendCommand(command,returnBuffer);
	if(!cs_success) printf("ERROR command: %s was not send or received correctly\n",command);
	sscanf(returnBuffer, "%d", &param);
	return param;
}

int cs_sendCommand_bool(char* command) {
	//printf("cs_send_bool: command = %s\n", command);
	cs_success = cs_sendCommand(command,returnBuffer);
	if(!cs_success) printf("ERROR command: %s was not send or received correctly\n",command);
	if(strcmp(returnBuffer,"false")==0) {
		return 0;
	}
	else if(strcmp(returnBuffer,"true")==0) {
		return 1;
	}
	else {
		printf("ERROR: unable to parse return value (%s) commandbuffer was = %s \n",returnBuffer,cs_commandBuffer);
		//cs_reconnect();
		return 0;
	}
}

float cs_sendCommand_float(char* command) {
	float param;
	cs_success = cs_sendCommand(command,returnBuffer);
	if(!cs_success) printf("ERROR command: %s was not send or received correctly\n",command);
	sscanf(returnBuffer, "%f", &param);
	return param;
}

char* cs_sendCommand_string(char* command, char* returnString) {
	cs_success = cs_sendCommand(command,returnBuffer);
	if(!cs_success) printf("ERROR command: %s was not send or received correctly\n",command);
	memcpy(returnString,returnBuffer,strlen(returnBuffer)+1);
	return returnString;
}

int cs_wasSuccess() {
	return cs_success;
}
