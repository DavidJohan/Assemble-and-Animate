#include <ase/infrastructure/EventManager/EventManager.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <ase/communication/CommDefs.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/Infrastructure.h>

static EventManager_t manager;

static listnode_t* findTopic(char* topic) {
	if(topic==NULL) return NULL;
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

//Java's hash algorithm from http://en.wikipedia.org/wiki/Java_hashCode%28%29
static short topicToHash(char* topic) {
	short hash =0;
	for(int i=0;i<sizeof(topic);i++) {
		hash = 31 * hash + topic[i];
	}
	return hash;
}

static char* hashToTopic(short hash) {
	listnode_t* node = manager.topics;
	while(node!=NULL) {
		char* topic = ((char*)node->data);
		if(topicToHash(topic) == hash) {
			return node->data;
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
	if(hashToTopic(topicToHash(topic))!=NULL) {
		ase_printf("Topic %s does not have a unique hash %i!\n",topic, topicToHash(topic));
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
	if(event!=NULL) event->info.topic = topic;
	PubSubManager_publish((PubSubManager_t*) &manager, (void*) topic, (void*) event, subscribes, publish);
}

void EventManager_publishInternal(char* topic, char* eventData, char eventDataSize) {
	Event_t event;
	event.val_prt = eventData; //looses info about channel!
	event.info.msgChannel = -1;
	event.info.dataSize = eventDataSize;
	EventManager_publish(topic, &event);
}

//all topics must be known at compile time - is this a problem?
void EventManager_publishLocal(char* topic, char* eventData, char eventDataSize) {
	Event_Msg_Header_t header;
	header.type = EVENT_LOCAL_MESSAGE;
	header.topicHash = topicToHash(topic);
	char message[eventDataSize+sizeof(Event_Msg_Header_t)];
	for(int i=0;i<sizeof(Event_Msg_Header_t);i++) {
		message[i] = ((char*)&header)[i];
	}
	for(int i=0;i<eventDataSize;i++) {
		message[i+sizeof(Event_Msg_Header_t)] = eventData[i];
	}
	sendMessageToAllNeighbors(message, sizeof(message));
}

void EventManager_publishGlobal(char* topic, char* eventData, char eventDataSize) {
	Event_Global_Msg_Header_t header;
	header.type = EVENT_GLOBAL_MESSAGE;
	header.topicHash = topicToHash(topic);
	header.timestamp = getLocalMsTime()%65536;
	char message[eventDataSize+sizeof(Event_Global_Msg_Header_t)];
	for(int i=0;i<sizeof(Event_Global_Msg_Header_t);i++) {
		message[i] = ((char*)&header)[i];
	}
	for(int i=0;i<eventDataSize;i++) {
		message[i+sizeof(Event_Global_Msg_Header_t)] = eventData[i];
	}
	broadcaster_broadcast(message, eventDataSize+sizeof(Event_Global_Msg_Header_t),-1);
}

void event_local_msg_handler(Msg_t* msg) {
	if(msg->messageSize>=sizeof(Event_Msg_Header_t)) {
		Event_Msg_Header_t* header = (Event_Msg_Header_t*) msg->message;
		char* topic = hashToTopic(header->topicHash);
		if(topic!=NULL) {
			Event_t event;
			event.val_prt = &(msg->message[sizeof(Event_Msg_Header_t)]); //looses info about channel!
			event.info.msgChannel = msg->channel;
			event.info.dataSize = msg->messageSize - sizeof(Event_Msg_Header_t);
			EventManager_publish(topic, &event);
		}
		else {
			ase_printf("ERROR: Got event with unknown topic hash %i\n",header->topicHash); //all non-internal topics should be registered with EventManager_registerTopic(...);
		}
	}
	else {
		//corrupt message received ?!
	}
}

void event_global_msg_handler(Msg_t* msg) {
	if(msg->messageSize>=sizeof(Event_Global_Msg_Header_t)) {
		Event_Global_Msg_Header_t* header = (Event_Global_Msg_Header_t*) msg->message;
		char* topic = hashToTopic(header->topicHash);
		if(topic!=NULL) {
			Event_t event;
			event.val_prt = &(msg->message[sizeof(Event_Global_Msg_Header_t)]); //looses info about channel!			event.info.msgChannel = msg->channel;
			event.info.dataSize = msg->messageSize - sizeof(Event_Global_Msg_Header_t);
			EventManager_publish(topic, &event);
			//header->hopCount++; //this will corrupt cache
			broadcaster_broadcast(msg->message, msg->messageSize, msg->channel);
		}
		else {
			ase_printf("ERROR: Got event with unknown topic hash %i\n",header->topicHash); //all non-internal topics should be registered with EventManager_registerTopic(...);
		}
	}
	else {
		//corrupt message received ?!
	}
}

void EventManager_init() {
	if(!PubSubManager_isInitialized((PubSubManager_t*) &manager)) {
		PubSubManager_init((PubSubManager_t*) &manager, NO_LABEL);
	}
	MsgManager_subscribe(EVENT_LOCAL_MESSAGE, event_local_msg_handler);
	MsgManager_subscribe(EVENT_GLOBAL_MESSAGE, event_global_msg_handler);
	broadcaster_init(); //Correct place to initialize broadcaster?
}
