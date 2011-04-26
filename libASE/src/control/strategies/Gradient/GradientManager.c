#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/EventManager/EventManager.h>

static GradientManager_t m;
static GradientManager_t* manager = &m;

static void act(char* topic, Event_t* event) {
	listnode_t* cNode = manager->gradients;
	while(cNode!=NULL) {
		Gradient_t* grad = cNode->data;
		Gradient_sendIfTime(grad, &(manager->msg));
		cNode = cNode->next;
	}
}

static void updateFromMsg(GradientMsg_t* gradientMsg) {
	listnode_t* cNode = manager->gradients;
	while(cNode!=NULL) {
		Gradient_t* grad = cNode->data;
		if(grad->label==gradientMsg->super.label) {
			Gradient_updateFromValue(grad, gradientMsg->value);
		}
		cNode = cNode->next;
	}
}

static void handleMessage(Msg_t* msg) {
	GradientMsg_t* gradientMsg = ((GradientMsg_t*) msg->message);
	updateFromMsg(gradientMsg);
}

static void initialize() {
	Manager_init((Manager_t*)manager, NO_LABEL);
	MsgManager_subscribe(GRADIENT_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
}

Gradient_t* GradientManager_createGradient(short label, float hz) {
	if(!Manager_isInitialized((Manager_t*)manager)) initialize();

	Gradient_t* gradient = MemManager_malloc(sizeof(Gradient_t));
	Gradient_initialize(gradient, label, hz);
	if(manager->gradients==NULL) {
		manager->gradients = LinkedList_createList(gradient);
	}
	else {
		LinkedList_insertAfter(manager->gradients, gradient);
	}
	return gradient;
}

