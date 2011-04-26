#include <stdlib.h>
#include <stdio.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>


static Event_t actEvent;
static Event_t msgRecvEvent;
static Msg_t msg;

void ase_init(void (*controller_init)()) {
	EventManager_registerTopic(INIT_DONE_EVENT);
	EventManager_registerTopic(ACT_EVENT);
	EventManager_registerTopic(MSG_RECV_EVENT);
	controller_init();
	actEvent.val_int=0;
	EventManager_publish(INIT_DONE_EVENT, NULL);
}

void ase_act() {
	actEvent.val_int++;
	EventManager_publish(ACT_EVENT, &actEvent);
}

void ase_handleMessage(char* message, char messageSize, char channel) {
	msg.message = message;
	msg.messageSize = messageSize;
	msg.channel = channel;
	msgRecvEvent.val_prt = &msg;
	EventManager_publish(MSG_RECV_EVENT, &msgRecvEvent);
}
