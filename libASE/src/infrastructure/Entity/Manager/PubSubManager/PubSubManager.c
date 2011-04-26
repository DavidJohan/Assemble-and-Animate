#include <stdlib.h>
#include <ase/infrastructure/Entity/Manager/PubSubManager/PubSubManager.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/infrastructure/Entity/Entity.h>
#include <ase/tools/LinkedList/LinkedList.h>

void PubSubManager_init(PubSubManager_t* manager, short label) {
	Entity_init((Entity_t*) manager,label);
}

bool PubSubManager_isInitialized(PubSubManager_t* manager) {
	return Entity_isInitialized((Entity_t*) manager);
}

void PubSubManager_addSubscriber(PubSubManager_t* manager, void* data) {
	if(manager->subscribers==NULL) {
		manager->subscribers = LinkedList_createList(data);
	}
	else {
		LinkedList_insertAfter(manager->subscribers, data);
	}
}

int PubSubManager_countSubscribers(PubSubManager_t* manager) {
	return LinkedList_size(manager->subscribers);
}

bool PubSubManager_hasSubscriber(PubSubManager_t* manager, void* subscriber, bool (*equals)(void*,void*)) {
	return LinkedList_contains(manager->subscribers, subscriber, equals);
}

int PubSubManager_publish(PubSubManager_t* manager, void* topic, void* event, bool (*subscribes)(void*,void*), void (*handler)(void*,void*,void*)) {
	int count = 0;
	listnode_t* node = LinkedList_findNext(manager->subscribers, topic, subscribes);
	while(node!=NULL) {
		handler(topic, event, node->data);
		node = LinkedList_findNext(node->next, topic, subscribes);
		count++;
	}
	return count;
}

