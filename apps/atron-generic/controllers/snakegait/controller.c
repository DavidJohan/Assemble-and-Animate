#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/control/behaviors/atron/locomotion/SnakeGait/SnakeGait.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

void act(char* topic, Event_t* event)  { }

void handleMessage(char* topic, Event_t* event) {
	if(message[0]==DISTRIBUTED_STATE_MESSAGE) {
		DSManager_handleMessage(message, messageSize, channel);
	}
}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	SnakeGait_init(0);
}
