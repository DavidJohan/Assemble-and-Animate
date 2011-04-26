#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/DynamixelApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>


void moveRandom(int id) {
	if(!dynamixelApi_isMoving(id)) {
		int cPos = dynamixelApi_getPosition(id);
		int pos = rand()%1024;
		ase_printf("Dynamixel %i at %i going to %i\n",id, cPos,pos);
		dynamixelApi_setGoalPos(id, pos);
	 }
}

void act(char* topic, Event_t* event)  {
	int i;
	for(i=0;i<dynamixelApi_countActuators();i++) {
		moveRandom(i);
	}
}

void handleMessage(char* topic, Event_t* event) { }

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	srand(getRandomSeed());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	dynamixelApi_connect(1);
	dynamixelApi_connect(2);
}
