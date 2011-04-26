#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpgManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

CnoCpg_t* cpg1;
int toRunningLeds(int val) {
	if(val<=0) return 0x00;
	else if(val<=32) return 0x01;
	else if(val<=64) return 0x03;
	else if(val<=95) return 0x07;
	else if(val<=127) return 0x0F;
	else if(val<=159) return 0x1F;
	else if(val<=191) return 0x3F;
	else if(val<=223) return 0x7F;
	else return 0xFF;
}

void cpg_updated(char* topic, Event_t* event)  {
	int cpgX = (int) (255.0*(1.0 + CpgSuper_getX((CpgSuper_t*)cpg1))/2.0);
	ase_printf("%i: %i at %f sec\n", getHardwareID(), cpgX, getLocalTime());
	atronApi_setLeds(toRunningLeds(cpgX));
}

void handleMessage(char* topic, Event_t* event) { }



void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(CPG_UPDATED_EVENT,cpg_updated);
	cpg1 = CnoCpgManager_createCpg(1, 0.2);
	CnoCpgManager_createCoupling(1, 1, 1, 0);
	Strategy_start((Strategy_t*)cpg1, NULL);
}
