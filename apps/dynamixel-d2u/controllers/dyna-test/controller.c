#include <stdio.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CpgSuperManager.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>

static void handleMessage(char* topic, Event_t* event) {}

int state =0;
int delay =0;
long lastUpdate = 0;
static void rotDone(char* topic, Event_t* event) {
	int actID = event->val_int;
	ase_printf("EVENT: Rotation of actuator %i done now at pos %i\n", actID, dynamixelApi_getPosition(actID));
}

static void act(char* topic, Event_t* event) {
	if(getLocalMsTime()<=(delay+lastUpdate)) {
		return;
	}
	else {
		lastUpdate = getLocalMsTime();
	}

	if(state==0) {
		ase_printf("Rotating clockwise full speed \n");
		dynamixelApi_setWheelMode(0, true);
		dynamixelApi_wheelMove(0, 1023, false);
		delay = 5000;	state++;
	}
	else if(state==1) {
		ase_printf("Rotating counter-clockwise half speed \n");
		dynamixelApi_wheelMove(0, 512, true);
		delay = 5000;	state++;
	}
	else if(state==2) {
		ase_printf("Rotating to mid-point from %i\n", dynamixelApi_getPosition(0));
		dynamixelApi_setWheelMode(0, false);
		dynamixelApi_setGoalPos(0,512);
		delay = 5000;	state = 0;
	}
}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	EventManager_subscribe(DYNAMIXEL_ROTATION_DONE_EVENT, rotDone);

	dynamixelApi_connect(1);
	dynamixelApi_setLed(0, 1);
}
