#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Entity/Entity.h>
#include <ase/infrastructure/Entity/Strategy/Strategy.h>
#include <ase/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

//goes partly up
bool CpgSuper_updateIfTime(CpgSuper_t* cpg, void (*iterate)(CpgSuper_t*)) {
	bool updated=false;
	if(!Strategy_isPaused((Strategy_t*)cpg) && Strategy_isStarted((Strategy_t*)cpg)) {
		while(PeriodicStrategy_hasPendingSteps((PeriodicStrategy_t*) cpg)) { //if time to update
			iterate(cpg);
			PeriodicStrategy_step((PeriodicStrategy_t*) cpg);
			updated=true;
		}
		if(updated) {
			Event_t event;
			event.val_prt = cpg;
			EventManager_publish(CPG_UPDATED_EVENT, &event);
		}
	}
	return updated;
}
void CpgSuper_setFrequency(float hz, CpgSuper_t* cpg) {
	int newDelay = (int) (1000.0f/(hz*((float)cpg->updatesPerperiode)));
	PeriodicStrategy_setDelay(newDelay, (PeriodicStrategy_t*) cpg);
	PeriodicStrategy_catchUp((PeriodicStrategy_t*) cpg);
}

float CpgSuper_getFrequency(CpgSuper_t* cpg) {
	float delay = (float)PeriodicStrategy_getDelay((PeriodicStrategy_t*) cpg);
	return 1000.0f/(delay*((float)cpg->updatesPerperiode));
}

void CpgSuper_resetState(CpgSuper_t *cpg) {
	cpg->x = 0.1f;
	cpg->y = 0.1f;
	cpg->nCouplings = 0;
}

void CpgSuper_sendState(CpgSuper_t* cpg, CpgSuperMsg_t* msg) {
	msg->super.label = Entity_getLabel((Entity_t*) cpg);
	msg->x = cpg->x;
	msg->y = cpg->y;
	sendMessageToAllNeighbors((char*)msg, sizeof(CpgSuperMsg_t));
}

float CpgSuper_getX(CpgSuper_t *cpg) {
	return cpg->ampl*cpg->x+cpg->x_offset;
}

float CpgSuper_getY(CpgSuper_t *cpg) {
	return cpg->ampl*cpg->y+cpg->y_offset;
}

void CpgSuper_setAmplitude(CpgSuper_t *cpg, float amplitude) {
	cpg->ampl = amplitude;
}

float CpgSuper_getAmplitude(CpgSuper_t *cpg) {
	return cpg->ampl;
}

float CpgSuper_getOffsetX(CpgSuper_t *cpg) {
	return cpg->x_offset;
}

float CpgSuper_getOffsetY(CpgSuper_t *cpg) {
	return cpg->y_offset;
}

void CpgSuper_setOffset(CpgSuper_t *cpg, float xOffset, float yOffset) {
	cpg->x_offset = xOffset;
	cpg->y_offset = yOffset;
}

void CpgSuper_setState(CpgSuper_t *cpg, float new_x, float new_y) {
	cpg->x = new_x;
	cpg->y = new_y;
}

void CpgSuper_setApplyToChannel(CpgSuperCoupling_t* coupling, int channel, int applyTo) {
	coupling->apply_to_channels[channel] = applyTo;
}

void CpgSuper_setStrength(CpgSuperCoupling_t* coupling, float strength) {
	coupling->strength = strength;
}

void CpgSuper_setPhaseDifference(CpgSuperCoupling_t* coupling, float phaseDifference) {
	coupling->phaseDiff = phaseDifference;
}
float CpgSuper_getPhaseDifference(CpgSuperCoupling_t* coupling) {
	return coupling->phaseDiff;
}
float CpgSuper_getStrength(CpgSuperCoupling_t* coupling) {
	return coupling->strength;
}

static void reset(void* data) {
	CpgSuper_resetState((CpgSuper_t*) data);
}

void CpgSuper_initialize(CpgSuper_t *cpg, uint8_t label, float hz) {
	Strategy_init((Strategy_t*)cpg, (short) label);
	Strategy_setRestartHandler((Strategy_t*)cpg, reset);
	PeriodicStrategy_init(1000, 0, (PeriodicStrategy_t*)cpg);
	cpg->ampl = 1.0f;
	cpg->x_offset = 0.0f;
	cpg->y_offset = 0.0f;
	cpg->updatesPerperiode = 100;//25;
	CpgSuper_setFrequency(hz, cpg);
	CpgSuper_resetState(cpg);
}
