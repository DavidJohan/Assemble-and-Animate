#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CpgSuperManager.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>


HCCpg_t* cpg;
static void cpg_updated(char* topic, Event_t* event)  {
	if(event->val_prt==cpg) {
		float x = CpgSuper_getX((CpgSuper_t*) cpg);
		float y = CpgSuper_getY((CpgSuper_t*) cpg);
		ase_printf("{%i, %f, %f, %f},\n", getHardwareID(), getLocalTime(), x, y);
	}
}

static void handleMessage(char* topic, Event_t* event) {}
static void act(char* topic, Event_t* event) {}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	EventManager_subscribe(CPG_UPDATED_EVENT,cpg_updated);

	cpg = HCCpgManager_createCpg(1, 0.1f);
	HCCpgManager_createCoupling(1, 1, 0.5, 0);
	Strategy_start((Strategy_t*)cpg, NULL);

	if(getHardwareID()==0) {
		CpgSuper_setState((CpgSuper_t*)cpg, 0.0f, 1.0f);
		CpgSuper_setOffset((CpgSuper_t*)cpg, 0.5f, 0.0f);
		CpgSuper_setAmplitude((CpgSuper_t*)cpg, 2.0f);
	}
	if(getHardwareID()==1) {
		CpgSuper_setState((CpgSuper_t*) cpg, 1.0f, 0.0f);
	}
}
