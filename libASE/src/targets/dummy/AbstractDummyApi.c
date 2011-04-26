#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <ase/targets/dummy.h>

static float startTime=0;

static float getTime() {
	time_t rawtime;
	struct tm *ptm;
	time (&rawtime);
	ptm = gmtime( &rawtime );
	return (float)(60.0f * ptm->tm_min + ptm->tm_sec);
  
	//return (float)time(NULL);
}

void AbstractDummyApi_init() {
	startTime = getTime();
}

float getLocalTime() {
	return getTime()-startTime;
}

long getLocalMsTime() {
	return (long)(1000*getLocalTime());
}

int sendMessageToAllNeighbors(char* msg, int length) {
	 printf("sendMessageToAllNeighbors");
	 return 0;
}

long getRandomSeed() {
	return time(NULL);
}

int sendMessageToNeighbor(char* msg, int length, int channel) {
	printf("sendMessageToNeighbor");
	return 0;
} 

int sendMessageToAllExcept(char* msg, char length, char except) {
	printf("sendMessageToAllExcept");
	return 0;
}
 
int getNumberOfNeighborChannels() {
	printf("getNumberOfNeighborChannels");
	return 0;
}

int hasNeighbor(int channel) {
	return 0;
}

int getHardwareID() {
	return 0;
}
