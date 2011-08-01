#include <stdio.h>
#include <math.h>
#include <ase/Infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>

#define ID_LOCAL_EVENT	"ID_LOCAL_EVENT"
#define ID_GLOBAL_EVENT	"ID_GLOBAL_EVENT"
#define N_MODULES	10
#define N_CHANNELS	8

char neighborIds[N_CHANNELS];
char robotIds[N_MODULES];

typedef struct {
	int id;
} IDEvent_t;
IDEvent_t idEvent;


void initTopologyState() {
	for(int i=0;i<N_CHANNELS;i++) {
		neighborIds[i] = -1;
	}
	for(int i=0;i<N_MODULES;i++) {
		robotIds[i] = -1;
	}
}

void printTopologyState() {
	ase_printf("Module %i: [", getHardwareID());
	for(int i=0;i<N_CHANNELS;i++) {
		ase_printf("%i, ", neighborIds[i]);
	}
	ase_printf("] ----- {");
	for(int i=0;i<N_MODULES;i++) {
		if(robotIds[i]!=-1) ase_printf("%i, ", i);
	}
	ase_printf("}\n\n");
}

void id_local_event_fired(char* topic, Event_t* event) {
	char neighborID = ((IDEvent_t*) event->val_prt)->id;
	neighborIds[(unsigned char)event->info.msgChannel] = neighborID;
}
void id_global_event_fired(char* topic, Event_t* event) {
	unsigned char moduleID = ((IDEvent_t*) event->val_prt)->id;
	robotIds[moduleID] = event->info.msgChannel;
}

void periodic_timer_fired(int id) {
	idEvent.id = getHardwareID();
	EventManager_publishLocal(ID_LOCAL_EVENT, (char*)&idEvent, sizeof(IDEvent_t));
	EventManager_publishGlobal(ID_GLOBAL_EVENT, (char*)&idEvent, sizeof(IDEvent_t));
	printTopologyState();
}

void controller_init() {
	EventManager_registerTopic(ID_LOCAL_EVENT);
	EventManager_registerTopic(ID_GLOBAL_EVENT);
	EventManager_subscribe(ID_LOCAL_EVENT, id_local_event_fired);
	EventManager_subscribe(ID_GLOBAL_EVENT, id_global_event_fired);
	TimerManager_createPeriodicTimer(1000, 0, periodic_timer_fired);
	initTopologyState();
}


