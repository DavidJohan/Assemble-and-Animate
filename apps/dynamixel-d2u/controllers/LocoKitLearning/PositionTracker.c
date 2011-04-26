#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ase/utils/socket-tools/SimpleSocket.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/PositionTracker.h>
#define BUFFER_SIZE 100
static int sockfd;
static char receiveBuffer[BUFFER_SIZE];
static int currentPos=0;
static float lastUpdateTime=0;
static int port;
static char host[100];
static bool isReady=false;
static bool paused = false;

void *listenForPos() {
	printf("PositionTracker: Connecting to %s on port %i...\n",host, port);
	sockfd = ss_connect(host, port);
	printf("PositionTracker: Successfully connected to host id=%i :-)\n",sockfd);
	while(1) {
		memset(receiveBuffer, 0, BUFFER_SIZE);
		ss_receive(receiveBuffer, sockfd);
		if(strlen(receiveBuffer)!=0)  {
			int pos = atoi(receiveBuffer);
			if(pos==50001) {
				printf("PositionTracker: Got command start!  \n");
				paused = false;
			}
			else if(pos==50002) {
				printf("PositionTracker: Got command pause!  \n");
				paused = true;
			}
			else {
				isReady=true;
				currentPos = pos; //scale to centimeters or meters?
				lastUpdateTime = getLocalTime();
			}
			if(currentPos<-10000) {
				printf("PositionTracker: Pause due to out of bounds!  \n");
				paused = true;
			}
		}
		else {
			printf("ERROR: In event listner null package received\n");
		}
	}
	printf("ERROR: Event thread exited\n");
	pthread_exit(0);
}
bool PositionTracker_isPaused() {
	return paused;
}

int PositionTracker_getCurrentPos() {
	return currentPos;
}
float PositionTracker_getLastUpdateTime() {
	return lastUpdateTime;
}
bool PositionTracker_isReady() {
	return isReady;
}
void PositionTracker_start(int port_, char* host_) {
	port=port_;
	strcpy(host,host_);
	pthread_t listenThread;
	pthread_create(&listenThread, NULL, listenForPos, NULL);
}
