#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>

static listnode_t* timers = NULL;
static bool initialized = false;
static void act(char* topic, Event_t* event) {
	long currentTime = getLocalMsTime();
	uint8_t firedCount=0;

	listnode_t* node = timers;
	while(node!=NULL) {
		Timer_t* timer = (Timer_t*) node->data;

		if(timer->isUsed && timer->isRunning) {
			if(currentTime>=timer->timeOutTime) {
                                timer->handler(timer->id); //call back, signal fired event
				if(timer->isPeriodic)  {
					timer->timeOutTime += timer->timeDelay;
				}
                                else if(timer->isNShots && timer->nShots>0) {
                                        timer->timeOutTime += timer->timeDelay;
                                        timer->nShots--;
                                }
				else {
					timer->isRunning = 0;
				}
				firedCount++;
			}
		}
		node = node->next;
	}
	if(firedCount!=0) {
		Event_t event;
		event.val_int = firedCount;
		EventManager_publish(TIMER_FIRED_EVENT, &event);
	}
}

static void init() {
	EventManager_subscribe(ACT_EVENT,act);
	EventManager_registerTopic(TIMER_FIRED_EVENT);
	initialized=true;
}


static void save(Timer_t* timer) {
	if(timers==NULL) {
		timers = LinkedList_createList(timer);
	}
	else {
		LinkedList_insertAfter(timers, timer);
	}
}

Timer_t* TimerManager_createPeriodicTimer(long msDelay,  int id, void (handler)(int)) {
	if(initialized==false) init();
	Timer_t* timer = MemManager_malloc(sizeof(Timer_t));
	timer->isUsed = 1;
	timer->isRunning = 1;
	timer->isPeriodic = 1;
        timer->isNShots = 0;
        timer->nShots = 0;
	timer->id = id;
	timer->handler = handler;
	timer->timeDelay = msDelay;
	timer->timeOutTime = getLocalMsTime() + msDelay;
	save(timer);
	return timer;
}


Timer_t* TimerManager_createOneShotTimer(long msDelay, int id,  void (handler)(int)) {
	if(initialized==false) init();
	Timer_t* timer = TimerManager_createPeriodicTimer(msDelay, id, handler);
	if(timer!=NULL) {
		timer->isPeriodic = 0;
	}
	return timer;
}

Timer_t* TimerManager_createNShotTimer(long msDelay, int nShots, int id,  void (handler)(int)) {
	if(initialized==false) init();
	Timer_t* timer = TimerManager_createPeriodicTimer(msDelay, id, handler);
	if(timer!=NULL) {
                timer->isPeriodic = 0;
		timer->isNShots = 1;
                timer->nShots = nShots-1;
	}
	return timer;
}


int TimerManager_setDelay(Timer_t* timer, long msDelay) {
	if(timer->isUsed) {
		timer->timeDelay = msDelay;
		return 1;
	}
	return 0;
}

int TimerManager_setNShots(Timer_t* timer, int nShots) {
	if(timer->isUsed) {
		timer->nShots = nShots-1;
		return 1;
	}
	return 0;
}

int TimerManager_getRemaningShots(Timer_t* timer) {
	if(timer->isUsed) {
		return timer->nShots;
	}
	return 0;
}

int TimerManager_removeTimer(Timer_t* timer) {
	if(timer->isUsed) {
		timer->isUsed = 0;
		return 1;
	}
	return 0;
}

int TimerManager_resetTimer(Timer_t* timer) {
	if(timer->isUsed) {
		timer->timeOutTime = getLocalMsTime() + timer->timeDelay;
		return 1;
	}
	return 0;
}

int TimerManager_pauseTimer(Timer_t* timer) {
	if(timer->isUsed) {
		timer->isRunning = 0;
		return 1;
	}
	return 0;
}

int TimerManager_startTimer(Timer_t* timer) {
	if(timer->isUsed) {
		TimerManager_resetTimer(timer);
		timer->isRunning = 1;
		return 1;
	}
	return 0;
}

