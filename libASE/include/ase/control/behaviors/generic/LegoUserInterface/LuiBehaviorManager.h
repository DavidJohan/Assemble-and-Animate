#ifndef LUIBEHAVIORMANAGER_H_
#define LUIBEHAVIORMANAGER_H_

#define		MAX_LUI_BEHAVIORS		20
typedef struct {
	char brickID;
	void* data;
	void (*start)(void* data);
	void (*act)(signed char* input, char nInput, signed char* output, char nOutput, void* data);
	void (*stop)(void* data);
	char type;
} LuiBehavior_t;



void LuiBehaviorManager_addBehavior(char brickID, void* data, void (*start)(void* data), void (*act)(signed char*, char, signed char*, char, void*), void (*stop)(void*), char type, Subsumption_t* subsumptionProcess);
bool LuiBehaviorManager_isSimple(char behaviorId);
bool LuiBehaviorManager_isCompound(char behaviorId);
bool LuiBehaviorManager_isTrain(char behaviorId);
bool LuiBehaviorManager_isRecord(char behaviorId);
bool LuiBehaviorManager_isEmpty(char behaviorId);
void* LuiBehaviorManager_getData(char behaviorId);

#endif
