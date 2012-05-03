
#ifndef CM510BEHAVIORS_H_
#define CM510BEHAVIORS_H_
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <stdbool.h>

#define LEFT_FRONT_UPPER_ID			6
#define LEFT_FRONT_UPPER_INDEX		0

#define LEFT_FRONT_LOWER_ID			5
#define LEFT_FRONT_LOWER_INDEX		1

#define RIGHT_FRONT_UPPER_ID		8
#define RIGHT_FRONT_UPPER_INDEX		2

#define RIGHT_FRONT_LOWER_ID		15
#define RIGHT_FRONT_LOWER_INDEX		3

#define LEFT_BACK_UPPER_ID			1
#define LEFT_BACK_UPPER_INDEX		4

#define LEFT_BACK_LOWER_ID			16
#define LEFT_BACK_LOWER_INDEX		5

#define RIGHT_BACK_UPPER_ID			7
#define RIGHT_BACK_UPPER_INDEX		6

#define RIGHT_BACK_LOWER_ID			9
#define RIGHT_BACK_LOWER_INDEX		7


void playback_start(void* data);
void playback_stop(void* data);
void playback_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
} CM510Behavior_osscilate_t;
void osscilate_start(void* data);
void osscilate_stop(void* data);
void osscilate_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
	bool initialized;
} CM510Behavior_cpg_t;
void cpg_start(void* data);
void cpg_stop(void* data);
void cpg_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
} CM510Behavior_logPos_t;
void logPos_start(void* data);
void logPos_stop(void* data);
void logPos_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);
#endif 



