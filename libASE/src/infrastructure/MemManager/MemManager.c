#include <ase/infrastructure/MemManager/MemManager.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/targets/AbstractModuleApi.h>

static MemManager_t manager;

static void lockMemory(char* topic, Event_t* event) {
        manager.locked=1;
        ase_printf("Memory now locked, total usage = %i vs allocated = %i bytes\n",manager.index,ASE_MEMORY_ALLOCATED);
}

static void init() {
        Manager_init((Manager_t*) &manager, NO_LABEL);
        manager.locked=0;
        manager.index=0;
        EventManager_subscribe(INIT_DONE_EVENT,lockMemory);
}

void* MemManager_malloc(int size) {
        if(!Manager_isInitialized((Manager_t*) &manager)) {
                init();
        }
        if((manager.index+size)>ASE_MEMORY_ALLOCATED) {
                ase_printf("ERROR: out of static memory (%i bytes allocated)!!!! - allocate more at compile time...\n",ASE_MEMORY_ALLOCATED);
                ase_printf("...using dynamic memory instead\n");
                void* mem = malloc(size);
                if (mem == NULL) {
                	while(1) ase_printf("Out of memory... %i\n", size);
                }
                manager.index += size;
                return mem;
        }
        void* ptr = &manager.memory[manager.index];
        if(!manager.locked) {
                manager.index += size;
        }
        else {
                manager.index += size;
                ase_printf("ERROR: trying to allocate memory when locked (%i bytes used)!!!!\n", manager.index);
        }
        return ptr;
}

void *_MemManager_Xmalloc(size_t size, char *filename, int fileline)
{
        void *mem = malloc(size);
        if (mem == NULL) {
                ase_printf("xmalloc: Out of memory! (%s:%i)\n", filename, fileline);
				#ifndef LMF 
                exit(1);
				#else
				while(1);
				#endif
        }   

        return mem;
}
