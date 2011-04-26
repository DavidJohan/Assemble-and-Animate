#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

void act(char* topic, Event_t* event)  { }

void handleMessage(char* topic, Event_t* event) { }

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	CpgGait_init();
	CpgGait_setAmplitude(0.5f, 0);
	CpgGait_setOffset(0.5f, 0);
	CpgGait_setFrequency(0.1f, 0);
	CpgGait_setUseFeedback(0);

	//Some phase shifts for ATRON walker
	if(getHardwareID()==0) CpgGait_setPhaseShift(3.14f, 0);
	if(getHardwareID()==1) CpgGait_setPhaseShift(3.14f, 0);
	if(getHardwareID()==2) CpgGait_setPhaseShift(0, 0);
	if(getHardwareID()==3) CpgGait_setPhaseShift(0, 0);
}
