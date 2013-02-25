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
/*
 * WARNING: THE FOLLOWING CODE HAS !!NOT!! BEEN TESTED!
 * DO NOT RELY ON IT!
 */
/*
 * Removes first occourrence of data.
 *  Requires a pointer to the head of the list.
 *  Returns a pointer to the head of the list.
 */
listnode_t* LinkedList_remove(listnode_t* node, void* data, bool (*equals)(void*,void*)){
	listnode_t* head = node;
	listnode_t* curr = node;
	if(equals(data,head->data)){
		curr=head->next;
		free(head);
		return curr;
	}
	curr=curr->next;
	listnode_t* prev= node;
	while(curr!=NULL) {
		if(equals(data, curr->data)) {
			curr=curr->next;
			free(prev->next);
			prev->next=curr;
			return head;
		}
		curr= curr->next;
		prev=prev->next;
	}
	return head;
}
/*
 * WARNING: THE FOLLOWING CODE HAS !!NOT!! BEEN TESTED!
 * DO NOT RELY ON IT!
 */
/*
 * Adds an element to the last positon of the list.
 */
void LinkedList_addLast(listnode_t* head, void* data){
	listnode_t* newNode = createNode(data);
	while(head->next!=NULL)
		head=head->next;
	head->next=newNode;
}
/*
 * WARNING: THE FOLLOWING CODE HAS !!NOT!! BEEN TESTED!
 * DO NOT RELY ON IT!
 */
listnode_t* LinkedList_modify(listnode_t* head, void*oldData, void*newData, bool (*equals)(void*,void*)){
	listnode_t* p=NULL;
	p=LinkedList_findNext(head,oldData,equals);
	if(p==NULL)
		return NULL;
	p->data=newData;
	return head;
}
