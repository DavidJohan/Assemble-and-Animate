#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/dummy.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/EventManager/EventManager.h>

Timer_t* periodicTimer;
Timer_t* oneShotTimer;

void periodic_timer_fired(int id) {
	ase_printf("Periodic timer fired at %i \n", (int)getLocalMsTime());
}

void one_shot_timer_fired(int id) {
	ase_printf("One Shot timer fired at %i \n",  (int)getLocalMsTime());
	ase_printf("Removing periodic timer \n");
	TimerManager_removeTimer(periodicTimer);
}

void any_timer_fired(char* topic, Event_t* event) {
	ase_printf("Some timer fired at %i \n", (int)getLocalMsTime());
}

void controller_init() {
	ase_printf("Module running %i\n",getHardwareID());
	periodicTimer = TimerManager_createPeriodicTimer(1000, 0, periodic_timer_fired);
	oneShotTimer = TimerManager_createOneShotTimer(10000, 1, one_shot_timer_fired);
	EventManager_subscribe("TIMER_FIRED_EVENT",any_timer_fired);
}
