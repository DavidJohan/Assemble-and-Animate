#include <ase/infrastructure/Entity/Strategy/Strategy.h>
#include <ase/infrastructure/Entity/Entity.h>
#include <stdbool.h>
#include <stdlib.h>

void Strategy_init(Strategy_t* strategy, short label) {
	Entity_init((Entity_t*) strategy, label);
	strategy->start = NULL;
	strategy->pause = NULL;
	strategy->resume = NULL;
	strategy->stop = NULL;
	strategy->restart = NULL;
	strategy->started = false;
	strategy->paused = false;
}

void Strategy_stop(Strategy_t* strategy, void* data) {
	if(strategy->stop!=NULL) {
		strategy->stop(data);
	}
	strategy->started = false;
}

void Strategy_start(Strategy_t* strategy, void* data) {
	if(strategy->start!=NULL) {
		strategy->start(data);
	}
	strategy->started = true;
}

void Strategy_pause(Strategy_t* strategy, void* data) {
	if(strategy->pause!=NULL) {
		strategy->pause(data);
	}
	strategy->paused = true;
}

void Strategy_resume(Strategy_t* strategy, void* data) {
	if(strategy->resume!=NULL) {
		strategy->resume(data);
	}
	strategy->paused = false;
}


void Strategy_restart(Strategy_t* strategy, void* data){
	if(strategy->restart!=NULL) {
		strategy->restart(data);
	}
	strategy->started = false;
	strategy->paused = false;
}

bool Strategy_isInitialized(Strategy_t* strategy) {
	return Entity_isInitialized((Entity_t*) strategy);
}

bool Strategy_isStarted(Strategy_t* strategy) {
	return strategy->started;
}

bool Strategy_isPaused(Strategy_t* strategy) {
	return strategy->paused;
}

void Strategy_setStartHandler(Strategy_t* strategy, void (*start)(void*)) {
	strategy->start = start;
}

void Strategy_setPauseHandler(Strategy_t* strategy, void (*pause)(void*)) {
	strategy->pause = pause;
}

void Strategy_setResumeHandler(Strategy_t* strategy, void (*resume)(void*)) {
	strategy->resume = resume;
}

void Strategy_setStopHandler(Strategy_t* strategy, void (*stop)(void*)) {
	strategy->stop = stop;
}

void Strategy_setRestartHandler(Strategy_t* strategy, void (*restart)(void*)) {
	strategy->restart = restart;
}
