#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/infrastructure/Entity/Entity.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Entity/Strategy/Strategy.h>
#include <ase/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.h>
#include <ase/communication/CommDefs.h>
#include <ase/targets/AbstractModuleApi.h>
#include <stdlib.h>
#include <stdio.h>

static void sendState(Gradient_t *gradient, GradientMsg_t* msg) {
	msg->super.msgType = GRADIENT_MESSAGE;
	msg->super.label = gradient->label;
	msg->value = Gradient_getValue(gradient);
	sendMessageToAllNeighbors((char*) msg, sizeof(GradientMsg_t));
}

bool Gradient_sendIfTime(Gradient_t *gradient, GradientMsg_t* msg) {
	bool updated=false;
	if(!Strategy_isPaused((Strategy_t*) gradient) && Strategy_isStarted((Strategy_t*)gradient)) {
		while(PeriodicStrategy_hasPendingSteps((PeriodicStrategy_t*) gradient)) { //if time to update
			PeriodicStrategy_step((PeriodicStrategy_t*) gradient);
			updated=true;
		}
		if(updated) {
			sendState(gradient,msg);
		}
	}
	return updated;
}

static void signalUpdatedEvent(Gradient_t *gradient) {
	Event_t event;
	event.val_prt = gradient;
	EventManager_publish(GRADIENT_UPDATED_EVENT, &event);
}

short Gradient_updateFromValue(Gradient_t *gradient, short value) {
	if(!Strategy_isPaused((Strategy_t*) gradient) && Strategy_isStarted((Strategy_t*) gradient)) {
		if(!gradient->seed) {
			if(gradient->value<(value-1)) {
				gradient->value = value-1;
				signalUpdatedEvent(gradient);
			}
		}
	}
	return gradient->value;
}

bool Gradient_isSeed(Gradient_t *gradient) {
	return gradient->seed;
}

void Gradient_setSeed(Gradient_t *gradient, bool seed, short value) {
	gradient->seed = seed;
	gradient->value = value;
	signalUpdatedEvent(gradient);
}

short Gradient_getValue(Gradient_t *gradient) {
	return gradient->value;
}

static void restart(void* data) {
	Gradient_t* gradient = (Gradient_t*) data;
	gradient->value = 0;
	gradient->seed = false;
	signalUpdatedEvent(gradient);
}

void Gradient_initialize(Gradient_t *gradient, short label, float hz) {
	Strategy_init((Strategy_t*) gradient, label);
	Strategy_setRestartHandler((Strategy_t*) gradient,restart);
	PeriodicStrategy_init((int)(1000.0f/hz), 0, (PeriodicStrategy_t*)gradient);
	gradient->label = label;
	gradient->value = 0;
	gradient->seed = false;
}
