#include <ase/completeTest.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/communication/Message.h>
#include <ase/tools/atron/AtronStatus/AtronStatus.h>
#include <stdio.h>

int id;
static void act(char* topic, Event_t* event)  {
	int itteration = event->val_int;
	if(itteration%500==0) {
		AtronStatus_print();
	}
}

static void handleMessage(char* topic, Event_t* event) {
	Msg_t* msg = (Msg_t*) event->val_prt;
	ase_printf("Module %i got message of type %i with length %i on channel %i\n",id,msg->message[0],msg->messageSize,msg->channel);
}

void done(char* topic, Event_t* event) {
	ase_printf("%i: Done event!...\n",getHardwareID());
}

void controller_init() {
	id = getHardwareID();
	AtronStatus_init();
	completeTest_init(id);
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
	EventManager_subscribe(COMPLETETEST_DONE_EVENT,done);
	ase_printf("Module %i Initialized\n",id);
}

