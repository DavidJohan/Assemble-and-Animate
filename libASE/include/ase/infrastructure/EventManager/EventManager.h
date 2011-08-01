/**
\file EventManager.h

EventManager.h provides a publish/subscribe system of internal events between different components

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
\note This software is distributed under the BSD open-source license.
*/

#ifndef EVENTMANAGER_H_
#define EVENTMANAGER_H_
#include <ase/infrastructure/Entity/Manager/PubSubManager/PubSubManager.h>
#include <ase/tools/LinkedList/LinkedList.h>

typedef struct {
	char* topic;
	char msgChannel;
	char dataSize;
	//char hopcount;
	//char senderID;
} EventInfo_t;

typedef struct {
	EventInfo_t info;
	char val_char;
	int val_int;
	float val_float;
	void* val_prt;
} Event_t;

typedef struct {
	char* topic; //name identifying topic
	void (*handler)(char*, Event_t*); //topic name and a pointer to the publisher
} EventHandler_t;

/**
 * A manager of events
 *
 *\extends PubSubManager_t
 */
typedef struct {
	PubSubManager_t super;
	//listnode_t* subscribers;
	listnode_t* topics;
} EventManager_t;

typedef struct {
	char type;
	short topicHash;
} Event_Msg_Header_t;

typedef struct {
	char type;
	short topicHash;
	unsigned short timestamp;
	//char hopCount;
} Event_Global_Msg_Header_t;



/**
 * Allows a component (a subscriber) to subscribe to specific event topics
 * char* topic should point to a global variable
 */
void EventManager_subscribe(char* topic, void (*handler)(char*, Event_t*));

/**
 * Allows an component (publisher) to make an topic available for publication
 */
void EventManager_registerTopic(char* topic);

/**
 * Allows a component (a publisher) to signal (or publish) an event
 */
void EventManager_publish(char* topic, Event_t* event);

void EventManager_publishInternal(char* topic, char* eventData, char eventDataSize);
void EventManager_publishLocal(char* topic, char* eventData, char eventSize);
void EventManager_publishGlobal(char* topic, char* eventData, char eventSize);

/**
 *
 */
void EventManager_init();


#endif /* EVENTMANAGER_H_ */
