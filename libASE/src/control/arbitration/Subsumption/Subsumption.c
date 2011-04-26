#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <math.h>

void Subsumption_init(Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		sub->behaviors[i].id = 0;
		sub->behaviors[i].active = 0;
		sub->behaviors[i].priority = 0;
		sub->behaviors[i].act = 0;
		sub->behaviors[i].data = 0;
		sub->behaviors[i].was_active = 0;
	}
}
void Subsumption_addBehavior(char id, char active, char priority, void* data, void (start)(void* data), void (act)(signed char* input, char nInput, signed char* output, char nOutput, void* data),void (stop)(void* data), Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id==0) {
			sub->behaviors[i].id = id;
			sub->behaviors[i].active = active;
			sub->behaviors[i].priority = priority;
			sub->behaviors[i].data = data;
			sub->behaviors[i].start = start;
			sub->behaviors[i].act = act;
			sub->behaviors[i].stop = stop;
			sub->behaviors[i].was_active = 0;
			return;
		}
	}
	#ifdef PRINTF
	ase_printf("WARNING in Subsumption: Adding more behaviors than there is space reserved \n");
	#endif
}
void Subsumption_removeBehavior(char id, Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id==id) {
			sub->behaviors[i].id = 0;
			sub->behaviors[i].active = 0;
			sub->behaviors[i].priority = 0;
			sub->behaviors[i].data = 0;
			sub->behaviors[i].act = 0;
			sub->behaviors[i].was_active = 0;
			return;
		}
	}
	#ifdef PRINTF
	ase_printf("WARNING in Subsumption: Unable to remove behavior - does not exist \n");
	#endif
}
void Subsumption_deactivateAll(Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		sub->behaviors[i].active = false;
	}
}

bool Subsumption_hasActiveBehaviors(Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id!=0 && sub->behaviors[i].active) return true;
		if(sub->behaviors[i].id!=0 && sub->behaviors[i].was_active) return true;
	}
	return false;
}

void Subsumption_setActive(char id, char active, Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id==id) {
			sub->behaviors[i].active = active;
			return;
		}
	}
	#ifdef PRINTF
	ase_printf("WARNING in Subsumption: Unable to find behavior to change activitity \n");
	#endif
}
void Subsumption_setPriority(char id, char priority, Subsumption_t* sub) {
	int i;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id==id) {
			sub->behaviors[i].priority = priority;
			return;
		}
	}
	#ifdef PRINTF
	ase_printf("WARNING in Subsumption: Unable to find behavior to set priority\n");
	#endif
}
static int myCmp(Behavior_t* b0, Behavior_t* b1) {
	if(b0->priority==b1->priority) {
		#ifdef PRINTF
		ase_printf("WARNING in Subsumption: all behaviors must have unique priorities\n");
		#endif
		return 0;
	}
	else if(b0->priority>b1->priority) return 1;
	else return -1;
}
void Subsumption_act(signed char* input, char nInput, signed char* output, char nOutput, Subsumption_t* sub) {
	qsort(sub->behaviors, SUBSUMPTION_N_BEHAVIORS, sizeof(Behavior_t), (int(*)(const void*, const void*))myCmp);
	int i;
	for(i=0;i<nOutput;i++) output[i] = 0;
	for(i=0;i<SUBSUMPTION_N_BEHAVIORS;i++) {
		if(sub->behaviors[i].id!=0 && sub->behaviors[i].active==1) {
			if(!sub->behaviors[i].was_active) {
				sub->behaviors[i].start(sub->behaviors[i].data);	
			}
			sub->behaviors[i].act(input, nInput, output, nOutput, sub->behaviors[i].data);
			sub->behaviors[i].was_active = true;
		}
		else if(sub->behaviors[i].id!=0 && sub->behaviors[i].active==0 && sub->behaviors[i].was_active) {
			sub->behaviors[i].stop(sub->behaviors[i].data);
			sub->behaviors[i].was_active = false;
		}
	}
}

