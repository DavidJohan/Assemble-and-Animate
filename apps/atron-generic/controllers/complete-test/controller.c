#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/communication/Message.h>
#include <ase/tools/atron/AtronStatus/AtronStatus.h>
#include <ase/targets/atron/AtronApi.h>


Timer_t* periodicTimer;

int id;
int connectorState=0;
void controller_act(char* topic, Event_t* event)  {
	int itteration = event->val_int;
	if(itteration%500==0) {
		AtronStatus_print();
		if(connectorState==1) {
			atronApi_disconnectAll();
			connectorState=0;
		}
		else {
			atronApi_connectAll();
			connectorState=1;
		}

	}
}

void handleMessage(char* topic, Event_t* event) {
	Msg_t* msg = (Msg_t*) event->val_prt;
	ase_printf("Module %i got message of type %i with length %i on channel %i\n",id,msg->message[0],msg->messageSize,msg->channel);
}

void controller_init() {
	id = getHardwareID();
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT,controller_act);
	AtronStatus_init();
	ase_printf("Module %i Initialized\n",id);
	atronApi_connectAll(); connectorState=1;
}
