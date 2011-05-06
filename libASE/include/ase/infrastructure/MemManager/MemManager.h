#ifndef MEMMANAGER_H_
#define MEMMANAGER_H_


#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

/**
 * A manager of static ram memory
 *
 *\extends Manager_t
 */
typedef struct {
	Manager_t super;
	char memory[ASE_MEMORY_ALLOCATED];
	int index;
	bool locked;
} MemManager_t;

void* MemManager_malloc(int size);

#define MemManager_Xmalloc(size) _MemManager_Xmalloc(size, __FILE__, __LINE__)
void *_MemManager_Xmalloc(size_t size, char *filname, int linenumber);

#endif /* MEMMANAGER_H_ */
