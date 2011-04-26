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

#define PI			3.1415926f

HCCpg_t* cpg0;
HCCpg_t* cpg1;

void goForCpg(HCCpg_t* cpg, int id) {
	float v = CpgSuper_getY((CpgSuper_t*) cpg);
	int vel = (int)(512*(v+1));
	if(vel>1023) vel = 1023;
	else if(vel<0) vel = 0;
	dynamixelApi_setMovingSpeed(id, vel);

	float x = CpgSuper_getX((CpgSuper_t*) cpg);
	int pos = (int)(512*(x+1));
	if(pos>1023) pos = 1023;
	else if(pos<0) pos = 0;
	int cPos = dynamixelApi_getPosition(id);
	dynamixelApi_setGoalPos(id, pos);

	ase_printf("{%i, %f, %i, %i},\n", id, getLocalTime(), pos, cPos);
}

static void cpg_updated(char* topic, Event_t* event)  {
	if(event->val_prt==cpg0) {
		goForCpg(cpg0, 0);
	}
	if(event->val_prt==cpg1) {
		goForCpg(cpg1, 1);
	}
}

static void handleMessage(char* topic, Event_t* event) {}
static void act(char* topic, Event_t* event) {}

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	EventManager_subscribe(CPG_UPDATED_EVENT,cpg_updated);

	dynamixelApi_connect(1);
	dynamixelApi_connect(2);

	float pd = PI;
	cpg0 = HCCpgManager_createCpg(1, 0.15f);
	HCCpgManager_createCoupling(1, 2, 0.5, pd);

	cpg1 = HCCpgManager_createCpg(2, 0.15f);
	HCCpgManager_createCoupling(2, 1, 0.5, 2*PI-pd);

	Strategy_start((Strategy_t*)cpg0, NULL);
	Strategy_start((Strategy_t*)cpg1, NULL);
}
