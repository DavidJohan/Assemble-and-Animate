#include <ase/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.h>
#include <ase/targets/AbstractModuleApi.h>

static long getDelay(int msDelay, int msVariance) {
	long randomComponent = 0;//msVariance-rand()%(2*msVariance);
	return msDelay+randomComponent;
}

void PeriodicStrategy_setDelay(int delay, PeriodicStrategy_t* per) {
	per->delay = delay;
}
void PeriodicStrategy_setVariance(int variance, PeriodicStrategy_t* per) {
	per->variance = variance;
}

int PeriodicStrategy_getDelay(PeriodicStrategy_t* per) {
	return per->delay;
}

int PeriodicStrategy_getVariance(PeriodicStrategy_t* per) {
	return per->variance;
}

void PeriodicStrategy_setParameters(int delay, int variance, PeriodicStrategy_t* per) {
	per->delay = delay;
	per->variance = variance;
}

int PeriodicStrategy_getPendingSteps(PeriodicStrategy_t* per) {
	long tDiff = (per->nextUpdateTime -getLocalMsTime());
	return (int)(tDiff/per->delay);
}

bool PeriodicStrategy_hasPendingSteps(PeriodicStrategy_t* per) {
	return per->nextUpdateTime<=getLocalMsTime();
}

void PeriodicStrategy_step(PeriodicStrategy_t* per) {
	per->nextUpdateTime += getDelay(per->delay, per->variance);
}

void PeriodicStrategy_catchUp(PeriodicStrategy_t* per) {
	per->nextUpdateTime = getLocalMsTime() + getDelay(per->delay, per->variance);
}

void PeriodicStrategy_init(int delay, int variance, PeriodicStrategy_t* per) {
	PeriodicStrategy_setParameters(delay, variance, per);
	PeriodicStrategy_catchUp(per);
}
