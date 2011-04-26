#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ase/control/strategies/Cpg/CpgSuperManager.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpgManager.h>
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

CnoCpg_t* CnoCpgManager_createCpg(short label, float hz) {
	if(manager == NULL) initManager();
	CnoCpg_t* cpg = (CnoCpg_t*) _CpgSuperManager_createCpg(label, hz, manager);
	CnoCpg_initialize(cpg);
	return cpg;
}

CpgSuperCoupling_t* CnoCpgManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff) {
	if(manager == NULL) initManager();
	CpgSuperCoupling_t* coupling = _CpgSuperManager_createCoupling(fromLabel, toLabel, strength, phaseDiff, manager);
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
	CnoCpg_addCoupling(x, y, coupling, (CnoCpg_t*) cpg);
}

static void iterate(CpgSuper_t* cpg) {
	CnoCpg_iterate((CnoCpg_t*) cpg);
}

static void initManager() {
	CnoCpg_init();
	MsgManager_subscribe(CPG_CNO_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	manager = MemManager_malloc(sizeof(CpgSuperManager_t));
	manager->sizeOfCpg = sizeof(CnoCpg_t);
	manager->sizeOfCoupling = sizeof(CpgSuperCoupling_t); //just use default
	manager->addCoupling = addCoupling;
	manager->iterate = iterate;
	manager->msg.super.msgType = CPG_CNO_MESSAGE;
}
