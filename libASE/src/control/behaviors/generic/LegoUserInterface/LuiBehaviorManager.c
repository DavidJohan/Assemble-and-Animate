#include <stdbool.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>

LuiBehavior_t luiBehaviors[MAX_LUI_BEHAVIORS];
int LuiBehaviorManager_index = 0;

void LuiBehaviorManager_addBehavior(char brickID, void* data, void (*start)(void* data), void (*act)(signed char*, char, signed char*, char, void*), void (*stop)(void*), char type, Subsumption_t* subsumptionProcess) {
	luiBehaviors[LuiBehaviorManager_index].brickID = brickID;
	luiBehaviors[LuiBehaviorManager_index].data = data;
	luiBehaviors[LuiBehaviorManager_index].start = start;
	luiBehaviors[LuiBehaviorManager_index].act = act;
	luiBehaviors[LuiBehaviorManager_index].stop = stop;
	luiBehaviors[LuiBehaviorManager_index].type = type;
	LuiBehaviorManager_index++;
	if(type!='c') Subsumption_addBehavior(brickID, 0, 0, data, start, act, stop, subsumptionProcess); //compound is included in subsumption since it is expanded seperately
}

bool LuiBehaviorManager_isSimple(char behaviorId) {
  	int i;
	for(i=0;i<MAX_LUI_BEHAVIORS;i++) {
		if(luiBehaviors[i].brickID == behaviorId) return luiBehaviors[i].type=='s';
	}
	return false;
}

bool LuiBehaviorManager_isCompound(char behaviorId) {
  	int i;
	for(i=0;i<MAX_LUI_BEHAVIORS;i++) {
		if(luiBehaviors[i].brickID == behaviorId) return luiBehaviors[i].type=='c';
	}
	return false;
}

bool LuiBehaviorManager_isTrain(char behaviorId) {
	int i;
	for(i=0;i<MAX_LUI_BEHAVIORS;i++) {
		if(luiBehaviors[i].brickID == behaviorId) return luiBehaviors[i].type=='t';
	}
	return false;
}

bool LuiBehaviorManager_isRecord(char behaviorId) {
	int i;
	for(i=0;i<MAX_LUI_BEHAVIORS;i++) {
		if(luiBehaviors[i].brickID == behaviorId) return luiBehaviors[i].type=='r';
	}
	return false;
}

bool LuiBehaviorManager_isEmpty(char behaviorId) {
	return behaviorId == 0;
}


void* LuiBehaviorManager_getData(char behaviorId) {
	int i;
	for(i=0;i<MAX_LUI_BEHAVIORS;i++) {
		if(luiBehaviors[i].brickID == behaviorId) return luiBehaviors[i].data;
	}
	return 0;
}
