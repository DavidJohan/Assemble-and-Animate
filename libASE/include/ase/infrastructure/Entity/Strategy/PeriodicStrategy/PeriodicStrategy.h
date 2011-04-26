/**
\file PeriodicStrategy.h
PeriodicStrategy.h is a construct for ASE. Most strategies extend it. It contains methods and variables
to start, stop, pause, resume and restart the strategy.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maersk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef PERIODICSTRATEGY_H_
#define PERIODICSTRATEGY_H_
#include <stdbool.h>
#include <ase/infrastructure/Entity/Strategy/Strategy.h>
/**
 * This struct extends Strategy_t with variables for strategies which are periodicly
 * acting
 *
 * \extends Strategy_t
 */
typedef struct {
	Strategy_t super;
	int delay;
	int variance;
	long nextUpdateTime;
} PeriodicStrategy_t;

void PeriodicStrategy_setDelay(int delay, PeriodicStrategy_t* per);
void PeriodicStrategy_setVariance(int variance, PeriodicStrategy_t* per);
int PeriodicStrategy_getDelay(PeriodicStrategy_t* per);
int PeriodicStrategy_getVariance(PeriodicStrategy_t* per);
void PeriodicStrategy_setParameters(int delay, int variance, PeriodicStrategy_t* per);
int  PeriodicStrategy_getPendingSteps(PeriodicStrategy_t* per);
bool PeriodicStrategy_hasPendingSteps(PeriodicStrategy_t* per);
void PeriodicStrategy_step(PeriodicStrategy_t* per);
void PeriodicStrategy_catchUp(PeriodicStrategy_t* per);
void PeriodicStrategy_init(int delay, int variance, PeriodicStrategy_t* per);
#endif
