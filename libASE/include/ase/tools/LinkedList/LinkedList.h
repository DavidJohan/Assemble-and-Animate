#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

#include <stdbool.h>

typedef struct lnode {
    void* data;
    struct lnode *next;
} listnode_t;

listnode_t* LinkedList_createList(void* data);
listnode_t* LinkedList_getNext(listnode_t* node);
listnode_t* LinkedList_insertAfter(listnode_t* node, void* data);
listnode_t* LinkedList_findNext(listnode_t* node, void* data, bool (*equals)(void*,void*));
int LinkedList_size(listnode_t* node);
bool LinkedList_contains(listnode_t* node, void* data, bool (*equals)(void*,void*));


//void LinkedList_removeAfter(listnode_t* node);


#endif /* LINKEDLIST_H_ */
