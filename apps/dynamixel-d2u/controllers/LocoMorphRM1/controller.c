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
int wheelStyle = 1;
void wheelControl() {
	if(state==1) {
		//ase_printf("Rotating clockwise full speed \n");
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			if(!dynamixelApi_isWheelMode(i)) {
				dynamixelApi_setWheelMode(i, true);
				dynamixelApi_wheelMove(i, 1023, false);
			}
			//ase_printf("%i: Dynamixel at %i at time %li ms\n", i, dynamixelApi_getPosition(i),getLocalMsTime());
			ase_printf("{%li, %i},\n",getLocalMsTime(), dynamixelApi_getPosition(i));
		}
		delay = 0;
	}
}

void cpgControl() {
	if(state==1) {
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
		delay = 100;	state++;
	}
	else if(state==2) {
		int i;
		for(i=0;i<getNumberOfActuators();i++) {
			ase_printf("%i: Dynamixel at %i\n", i, dynamixelApi_getPosition(i));
		}
		delay = 100;
	}
}
void act(char* topic, Event_t* event)  {
	if(getLocalMsTime()<=(delay+lastUpdate)) {
		return;
	}
	else {
		lastUpdate = getLocalMsTime();
	}
	if(state==0) {
		dynamixelApi_setGoalPos(0,512);
		delay = 2000;	state++;
	}
	else {
		if(wheelStyle) {
			wheelControl();
			return;
		}
		else {
			cpgControl();
			return;
		}
	}
}

void handleMessage(char* topic, Event_t* event) { }

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);

	dynamixelApi_connect(1);
	//dynamixelApi_connect(2);
	CpgGait_init();
	CpgGait_setCouplingStrenghts(0.5f, 0.5f, 0.5f);
}
