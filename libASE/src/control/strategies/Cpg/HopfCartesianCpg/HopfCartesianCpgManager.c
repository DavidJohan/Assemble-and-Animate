#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ase/control/strategies/Cpg/CpgSuperManager.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/communication/Message.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

static CpgSuperManager_t* manager = NULL;
static void initManager();

HCCpg_t* HCCpgManager_createCpg(short label, float hz) {
	if(manager == NULL) initManager();
	HCCpg_t* cpg = (HCCpg_t*) _CpgSuperManager_createCpg(label, hz, manager);
	HCCpg_initialize(cpg);
	return cpg;
}

CpgSuperCoupling_t* HCCpgManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff) {
	if(manager == NULL) initManager();
	CpgSuperCoupling_t* coupling =  _CpgSuperManager_createCoupling(fromLabel, toLabel, strength, phaseDiff, manager);
	return coupling;
}

static void act(char* topic, Event_t* event) {
	_CpgSuperManager_update(manager);
}

static void handleMessage(Msg_t* msg) {
	CpgSuperMsg_t* cpgMsg = ((CpgSuperMsg_t*) msg->message);
	_CpgSuperManager_handleMessage(cpgMsg, msg->channel, manager);
}

static void addCoupling(float x, float y, CpgSuperCoupling_t* coupling, CpgSuper_t* cpg) {
	HCCpg_addCoupling(x, y, coupling, (HCCpg_t*) cpg);
}

static void iterate(CpgSuper_t* cpg) {
	HCCpg_iterate((HCCpg_t*) cpg);
}

static void initManager() {
	HCCpg_init();
	MsgManager_subscribe(CPG_HC_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	manager = MemManager_malloc(sizeof(CpgSuperManager_t));
	manager->sizeOfCpg = sizeof(HCCpg_t);
	manager->sizeOfCoupling = sizeof(CpgSuperCoupling_t);
	manager->addCoupling = addCoupling;
	manager->iterate = iterate;
	manager->msg.super.msgType = CPG_HC_MESSAGE;
}
