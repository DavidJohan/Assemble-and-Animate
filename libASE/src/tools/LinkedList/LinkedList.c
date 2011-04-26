#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

static listnode_t* createNode(void* data) {
	listnode_t* node = (listnode_t*) MemManager_malloc(sizeof(listnode_t));
	node->data = data;
	node->next = NULL;
	return node;
}

listnode_t* LinkedList_createList(void* data) {
	return createNode(data);
}

listnode_t* LinkedList_getNext(listnode_t* node) {
	return node->next;
}

int LinkedList_size(listnode_t* node) {
	listnode_t* cNode = node;
	int size =0;
	while(cNode!=NULL) {
		size++;
		cNode = cNode->next;
	}
	return size;
}

listnode_t* LinkedList_insertAfter(listnode_t* node, void* data) {
	listnode_t* newNode = createNode(data);
	if(node->next!=NULL) {
		newNode->next = node->next;
	}
	node->next = newNode;
	return newNode;
}

listnode_t* LinkedList_findNext(listnode_t* node, void* data, bool (*equals)(void*,void*)) {
	listnode_t* cNode = node;
	while(cNode!=NULL) {
		if(equals(data, cNode->data)) {
			return cNode;
		}
		cNode = cNode->next;
	}
	return NULL;
}

bool LinkedList_contains(listnode_t* node, void* data, bool (*equals)(void*,void*)) {
	if(LinkedList_findNext(node, data, equals)!=NULL) {
		return true;
	}
	else {
		return false;
	}
}
