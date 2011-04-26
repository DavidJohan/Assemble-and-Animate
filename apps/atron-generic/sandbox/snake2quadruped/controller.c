#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/control/behaviors/atron/self-reconfiguration/SnakeToQuadruped_8M/SnakeToQuadruped_8M.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>

void act(char* topic, Event_t* event)  {
	SnakeToQuadruped_8M_act();
	if(SnakeToQuadruped_8M_isDone()) {
		SnakeToQuadruped_8M_reset();
		printf("*********DONE*********");
	}
	if(DSManager_act()>0) { }
}

void handleMessage(char* topic, Event_t* event) { }

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	SnakeToQuadruped_8M_init(8-getHardwareID());
}
