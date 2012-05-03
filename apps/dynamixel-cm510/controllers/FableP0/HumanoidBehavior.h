
#ifndef HUMANOIDBEHAVIORS_H_
#define HUMANOIDBEHAVIORS_H_
#include <stdbool.h>

typedef struct {
	long startTime;
	bool playing;
} CM510Behavior_humanoid_t;
void humanoid_start(void* data);
void humanoid_stop(void* data);
void humanoid_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


#endif
