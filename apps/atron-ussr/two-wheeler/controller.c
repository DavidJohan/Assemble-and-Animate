#include <ase/targets/atron/AtronApi.h>
#include <stdio.h>
#include <stdlib.h>

#define PING		1
#define ROLESET		2

#define UNDEFINED	1
#define CENTER		2
#define LEFT		3
#define RIGHT		4


int role=UNDEFINED;
char centerChannel;
char moduleName[50];

int countConnections() {
	int count=0;
	int i;
	for(i=0;i<8;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			count++;
		}
	}
	return count;
}

void selectRole() {
	int nConnection = countConnections();
	if(nConnection==2) {
		printf("%s: I am the Center\n",moduleName);
		role = CENTER;
		int i, count=0;
		for(i=0;i<8;i++) {
			if(atronApi_isOtherConnectorNearby(i))  {
				char leftMsg[] = {ROLESET, LEFT};
				if(count==0) atronApi_sendMessage(leftMsg, 2, i); //select left 
				
				char rightMsg[] = {ROLESET, RIGHT};
				if(count==1) atronApi_sendMessage(rightMsg, 2, i); //select right	
				count++;
			}
		}
	}
	while(role==UNDEFINED) {  atronApi_yield(); }
}

char pingMsg[] = {PING, 0};
void pingNeighbors() {
	int i;
	for(i=0;i<8;i++) {
		if(atronApi_isOtherConnectorNearby(i))  {
			atronApi_sendMessage(pingMsg, 2, i); 
			pingMsg[1]++;
		}
	}
}

void mywait(int ms) { 
	float endTime = atronApi_getTime()+ms/1000.0f;
	while(endTime>atronApi_getTime()) { atronApi_yield(); }
}

void mainControlLoop() {
	int started = 0;
	while(1) {
		if(role==UNDEFINED) { 
			printf("%s: My role is undefined\n",moduleName);
		}
		if(role==CENTER) {
			mywait(10000);
			printf("%s: Current Time: %f - will now ping wheels \n",moduleName ,atronApi_getTime());
			pingNeighbors();
		}
		if(role==LEFT) { 
			if(!started) {
				char centerMsg[] = {ROLESET, CENTER};
				atronApi_sendMessage(centerMsg, 2, centerChannel); 
				atronApi_rotateContinuous(1);
				started = 1;
			}
		}
		if(role==RIGHT) { 
			if(!started) {
				char centerMsg[] = {ROLESET, CENTER};
				atronApi_sendMessage(centerMsg, 2, centerChannel); 
				atronApi_rotateContinuous(-1);
				started = 1;
			}
		}
		atronApi_yield();
	}
}

void controller_start()  {
	atronApi_getName(moduleName);
	printf("%s: Controller started\n", moduleName);
	mywait(1000);
	selectRole();
	mainControlLoop();
}

void handleMessage(char* message, char messageSize, char channel) {
	centerChannel = channel;
	if(message[0]==PING) {
		printf("%s: I was pinged with value %i \n", moduleName, message[1]);
	}
	if(message[0]==ROLESET) {
		role = message[1];
		if(role==LEFT) {
			printf("%s: I was told to be Left Wheel\n",moduleName);
		}
		if(role==RIGHT) {
			printf("%s: I was told to be Right Wheel\n",moduleName);
		}
		if(role==CENTER) {
			printf("%s: I was told to be Center Module\n",moduleName);
		}
	}
}
