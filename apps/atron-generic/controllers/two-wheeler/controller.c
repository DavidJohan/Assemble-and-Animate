#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

void act(char* topic, Event_t* event)  {
	if(getHardwareID()==0) atronApi_rotateContinuous(0);
	if(getHardwareID()==1) atronApi_rotateContinuous(1);
	if(getHardwareID()==2) atronApi_rotateContinuous(-1);
}

void handleMessage(char* topic, Event_t* event) {
	ase_printf("%i :got message\n", getHardwareID());
}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
}
