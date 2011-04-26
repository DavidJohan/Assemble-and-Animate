#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

static MsgManager_t manager;

static bool sameMsgType(void* sub1, void* sub2) {
	if(((MsgHandler_t*) sub1)->msgType==((MsgHandler_t*) sub2)->msgType) {
		return true;
	}
	else {
		return false;
	}
}

static bool sameHandler(void* sub1, void* sub2) {
	if(((MsgHandler_t*) sub1)->handler==((MsgHandler_t*) sub2)->handler) {
		return true;
	}
	else {
		return false;
	}
}

static bool equals(void* s1, void* s2) {
	if(sameMsgType(s1,s2) && sameHandler(s1,s2)) {
		return true;
	}
	else {
		return false;
	}
}

static bool subscribes(void* topic, void* sub) {
	Msg_t* message = (Msg_t*) topic;
	MsgHandler_t* subscriber = (MsgHandler_t*)sub;
	return message->message[0]==subscriber->msgType;
}

static void publish(void* topic, void* msg, void* sub) {
	Msg_t* message = (Msg_t*) msg;
	MsgHandler_t* subscriber = (MsgHandler_t*)sub;
	subscriber->handler(message);
}

static void handleMessage(char* topic, Event_t* event) {
	Msg_t* msg = (Msg_t*) event->val_prt;
	PubSubManager_publish((PubSubManager_t*) &manager, (void*) msg, (void*) msg, subscribes, publish);
}

static void init() {
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	PubSubManager_init((PubSubManager_t*) &manager, NO_LABEL);
}

void MsgManager_subscribe(char msgType, void (*handler)(Msg_t*))  {
	if(!PubSubManager_isInitialized((PubSubManager_t*) &manager)) {
		init();
	}
	MsgHandler_t sub = {msgType, handler};
	if(!PubSubManager_hasSubscriber((PubSubManager_t*) &manager, &sub, equals)) {
		MsgHandler_t* data = MemManager_malloc(sizeof(MsgHandler_t));
		data->msgType = msgType;
		data->handler = handler;
		PubSubManager_addSubscriber((PubSubManager_t*) &manager,data);
	}
	else {
		ase_printf("Warning: msgType = %i with handler %i is already subscribed\n", msgType, (int)handler);
	}
}

