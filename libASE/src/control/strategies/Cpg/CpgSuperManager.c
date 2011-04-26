#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/control/strategies/Cpg/CpgSuperManager.h>

#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/config/ConfigASE.h>

#include <ase/communication/Message.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

CpgSuper_t* _CpgSuperManager_createCpg(short label, float hz, CpgSuperManager_t* manager) {
	CpgSuper_t* cpg = MemManager_malloc(manager->sizeOfCpg);
	CpgSuper_initialize(cpg, label, hz);
	if(manager->cpgs==NULL) {
		manager->cpgs = LinkedList_createList(cpg);
	}
	else {
		LinkedList_insertAfter(manager->cpgs, cpg);
	}
	return cpg;
}

CpgSuperCoupling_t* _CpgSuperManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff, CpgSuperManager_t* manager) {
	CpgSuperCoupling_t* coupling = MemManager_malloc(manager->sizeOfCoupling);
	coupling->from_label = fromLabel;
	coupling->to_label = toLabel;
	CpgSuper_setPhaseDifference(coupling, phaseDiff);
	CpgSuper_setStrength(coupling, strength);
	int j;
	for(j=0;j<MAX_NODE_DEGREE;j++) {
		CpgSuper_setApplyToChannel(coupling, j, 1);
	}
	if(manager->couplings==NULL) {
		manager->couplings = LinkedList_createList(coupling);
	}
	else {
		LinkedList_insertAfter(manager->couplings, coupling);
	}
	return coupling;
}

static void addCouplingFromMsg(CpgSuperMsg_t* cpgMsg, int channel, CpgSuperManager_t* manager) {
	listnode_t* lcoupling = manager->couplings;
	while(lcoupling!=NULL) {
		CpgSuperCoupling_t* coupling = ((CpgSuperCoupling_t*) lcoupling->data);
		if(coupling->from_label==cpgMsg->super.label) {
			listnode_t* lcpg = manager->cpgs;
			while(lcpg!=NULL) {
				CpgSuper_t* cpg = ((CpgSuper_t*) lcpg->data);
				if(coupling->to_label==Entity_getLabel((Entity_t*) cpg) && coupling->apply_to_channels[channel]==1) {
					manager->addCoupling(cpgMsg->x, cpgMsg->y, coupling, cpg);
				}
				lcpg = lcpg->next;
			}
		}
		lcoupling = lcoupling->next;
	}
}

static void addCouplingFromSelf(CpgSuper_t* sCpg, CpgSuperManager_t* manager) {
	listnode_t* lcoupling = manager->couplings;
	while(lcoupling!=NULL) {
		CpgSuperCoupling_t* coupling = ((CpgSuperCoupling_t*) lcoupling->data);
		if(coupling->from_label==Entity_getLabel((Entity_t*) sCpg) && coupling->from_label!=coupling->to_label) {
			listnode_t* lcpg = manager->cpgs;
			while(lcpg!=NULL) {
				CpgSuper_t* cpg = ((CpgSuper_t*) lcpg->data);
				if(coupling->to_label==Entity_getLabel((Entity_t*) cpg)) {
					manager->addCoupling(sCpg->x, sCpg->y, coupling, cpg);
				}
				lcpg = lcpg->next;
			}
		}
		lcoupling = lcoupling->next;
	}
}

void _CpgSuperManager_handleMessage(CpgSuperMsg_t* msg, char channel, CpgSuperManager_t* manager) {
	addCouplingFromMsg(msg, channel, manager);
}

int _CpgSuperManager_update(CpgSuperManager_t* manager) {
	int updateCount=0;
	listnode_t* lcpg = manager->cpgs;
	while(lcpg!=NULL) {
		CpgSuper_t* cpg = (CpgSuper_t*) lcpg->data;
		if(CpgSuper_updateIfTime(cpg, manager->iterate)) {
			addCouplingFromSelf(cpg, manager);
			CpgSuper_sendState(cpg, &(manager->msg));
			updateCount++;
		}
		lcpg = lcpg->next;
	}
	return updateCount;
}

