
#ifndef SNAKEBEHAVIORS_H_
#define SNAKEBEHAVIORS_H_
#include <stdbool.h>

typedef struct {
	long startTime;
	bool initialized;
} CM510Behavior_snake_t;
void snake_start(void* data);
void snake_stop(void* data);
void snake_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


#endif
