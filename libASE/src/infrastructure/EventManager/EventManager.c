#include <ase/infrastructure/EventManager/EventManager.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>

static EventManager_t manager;
static listnode_t* findTopic(char* topic) {
	listnode_t* node = manager.topics;
	while(node!=NULL) {
		char* rTopic = ((char*)node->data);
		if(strcmp(rTopic,topic) == 0) {
			return node;
		}
		node = node->next;
	}
	return NULL;
}

void EventManager_registerTopic(char* topic) {
	if(findTopic(topic)!=NULL) {
		ase_printf("Topic %s already exists\n",topic);
		return;
	}
	if(manager.topics==NULL) {
		manager.topics = LinkedList_createList(topic);
	}
	else {
		LinkedList_insertAfter(manager.topics, topic);
	}
	ase_printf("Topic %s now registered\n",topic);
}

static bool equals(void* s1, void* s2) {
	EventHandler_t* sub1 = (EventHandler_t*) s1;
	EventHandler_t* sub2 = (EventHandler_t*) s2;
	if(strcmp(sub1->topic,sub2->topic)==0 && sub1->handler==sub2->handler) {
		return true;
	}
	else {
		return false;
	}
}

void EventManager_subscribe(char* topic, void (*handler)(char*, Event_t*)) {
	if(!PubSubManager_isInitialized((PubSubManager_t*) &manager)) {
		PubSubManager_init((PubSubManager_t*) &manager, NO_LABEL);
	}
	EventHandler_t sub;
	sub.topic = topic;
	sub.handler = handler;

	if(!PubSubManager_hasSubscriber((PubSubManager_t*) &manager, &sub, equals)) {
		EventHandler_t* eventHandler = MemManager_malloc(sizeof(EventHandler_t));
		eventHandler->topic = topic;
		eventHandler->handler = handler;
		PubSubManager_addSubscriber((PubSubManager_t*) &manager,(void*) eventHandler);
	}
	else {
		ase_printf("Handler %i already subscribes to topic %s\n",(int) handler,topic);
		return;
	}
	ase_printf("Handler is now subscribing on topic %s \n",topic);
}

static bool subscribes(void* topic, void* sub) {
	EventHandler_t* s = (EventHandler_t*) sub;
	if(strcmp((char*)topic,s->topic)==0) {
		return true;
	}
	else {
		return false;
	}
}

static void publish(void* topic, void* event, void* sub) {
	EventHandler_t* s = (EventHandler_t*) sub;
	s->handler((char*) topic, (Event_t*) event);
}

void EventManager_publish(char* topic, Event_t* event) {
	//publish now? - chance of deadlocks, race conditions and heap overrun, what if one event triggers another?
	//publish later? - fixed size queue needed, may overrun and then what?
	PubSubManager_publish((PubSubManager_t*) &manager, (void*) topic, (void*) event, subscribes, publish);
}
