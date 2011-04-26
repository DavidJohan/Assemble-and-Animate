/**
 * \file Subsumption.h
 *
 *  Created on: 26 Jan, 2011
 *  \author David Johan Christensen
 */


#ifndef SUBSUMPTION_H_

#define SUBSUMPTION_N_BEHAVIORS	20
typedef struct {
	char id;
	char active;
	char priority;
	char was_active;
	void* data;
	void (*start)();
	void (*act)(signed char* input, char nInput, signed char* output, char nOutput, void* data);
	void (*stop)();
} Behavior_t;


typedef struct {
	Behavior_t behaviors[SUBSUMPTION_N_BEHAVIORS];
} Subsumption_t;


void Subsumption_init(Subsumption_t* sub);
void Subsumption_addBehavior(
		char id,
		char active,
		char priority,
		void* data,
		void (start)(void* data),
		void (act)(signed char* input, char nInput, signed char* output, char nOutput, void* data),
		void (stop)(void* data),
		Subsumption_t* sub);
void Subsumption_removeBehavior(char id, Subsumption_t* sub);
void Subsumption_setActive(char id, char active, Subsumption_t* sub);
void Subsumption_setPriority(char id, char priority, Subsumption_t* sub);
void Subsumption_act(signed char* input, char nInput, signed char* output, char nOutput, Subsumption_t* sub);
void Subsumption_deactivateAll(Subsumption_t* sub);
bool Subsumption_hasActiveBehaviors(Subsumption_t* sub);
#define SUBSUMPTION_H_

#endif
