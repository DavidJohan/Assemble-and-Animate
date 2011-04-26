#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>

int state =0;
int delay =0;
long lastUpdate = 0;
void act(char* topic, Event_t* event)  {
	if(getLocalMsTime()<=(delay+lastUpdate)) {
		return;
	}
	else {
		lastUpdate = getLocalMsTime();
	}
	if(state==0) {
		dynamixelApi_setGoalPos(0,512);
		dynamixelApi_setGoalPos(1,512);
		dynamixelApi_setGoalPos(2,512);
		dynamixelApi_setGoalPos(3,512);
		delay = 2000;	state++;
	}
	else if(state==1) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			CpgGait_setAmplitude(0.5f, i);
			CpgGait_setOffset(0.5f, i);
			CpgGait_setFrequency(1.0f, i);
			CpgGait_setUseFeedback(0);
			CpgGait_setPhaseShift(0, i);
		}
		if(!CpgGait_isStarted()) {
			CpgGait_start();
		}
		delay = 10000;	state++;
	}
	else if(state==2) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			if(i%2==0) CpgGait_setPhaseShift(0.0f, i);
			else CpgGait_setPhaseShift(3.14f, i);
		}
		delay = 10000;	state++;
	}
	else if(state==3) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			CpgGait_setPhaseShift(i*3.14f/6.0f, i);
		}
		delay = 10000;	state=1;
	}
}

void handleMessage(char* topic, Event_t* event) { }

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);

	dynamixelApi_connect(1);
	//dynamixelApi_connect(2);
	//dynamixelApi_connect(3);
	//dynamixelApi_connect(4);

	CpgGait_init();
	CpgGait_setCouplingStrenghts(0.5f, 0.5f, 0.5f);
}
