/**
\file PubSubManager/PubSubManager.h

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maersk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef PUBSUBMANAGER_H_
#define PUBSUBMANAGER_H_
#include <stdbool.h>
#include <ase/infrastructure/Entity/Entity.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/tools/LinkedList/LinkedList.h>

/**
 * This struct contains the variables and methods to manage components
 * \extends Manager_t
 */
typedef struct {
	Manager_t super;
	listnode_t* subscribers;
} PubSubManager_t;

void PubSubManager_init(PubSubManager_t* manager, short label);
bool PubSubManager_isInitialized(PubSubManager_t* manager);
int PubSubManager_countSubscribers(PubSubManager_t* manager);
void PubSubManager_addSubscriber(PubSubManager_t* manager, void* subscriber);
bool PubSubManager_hasSubscriber(PubSubManager_t* manager, void* subscriber, bool (*equals)(void*,void*));
int PubSubManager_publish(PubSubManager_t* manager, void* topic, void* event, bool (*subscribes)(void*,void*), void (*handler)(void*,void*,void*));

#endif /* PUBSUBMANAGER_H_*/
