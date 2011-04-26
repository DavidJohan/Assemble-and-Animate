/**
 * \file TimerManager.h
 *
 * TimerManager.h provides managed timers to applications and control strategies
 *
 * \date March 27, 2009
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */


#ifndef TIMERMANAGER_H_
#include <stdbool.h>
#include <stdint.h>
#include <ase/config/ConfigASE.h>

#define TIMER_FIRED_EVENT "TIMER_FIRED_EVENT"

typedef struct {
	uint8_t isUsed : 1;
	uint8_t isPeriodic : 1;
	uint8_t isRunning : 1;
	int id;
	long timeDelay;
	long timeOutTime;
	void (*handler)(int);
} Timer_t;

/**
 * Create a periodic timer which will call the handler periodic with a delay of msDelay in milliseconds.
 * The argument for the handler is the id parameter.
 */
Timer_t* TimerManager_createPeriodicTimer(long msDelay, int id, void (handler)(int));

/**
 * Create a one shot timer which will call the handler after a delay of msDelay in milliseconds.
 * The argument for the handler is the id parameter.
 */
Timer_t* TimerManager_createOneShotTimer(long msDelay, int id, void (handler)(int));

/**
 * Set the delay of the timer to fire after msDelay.
 * \return 1 if timer is in use
 */
int TimerManager_setDelay(Timer_t* timer, long msDelay);

/**
 * Remove a timer.
 * \return 1 if successful
 */
int TimerManager_removeTimer(Timer_t* timer);

/**
 * Reset a timer so that the timer will fire after msDelay milliseconds
 */
int TimerManager_resetTimer(Timer_t* timer);

/**
 * Stops the timer from generating call-back events
 */
int TimerManager_pauseTimer(Timer_t* timer);

/**
 * Start timer if it has been paused, will automatically reset the timer
 */
int TimerManager_startTimer(Timer_t* timer);
#define TIMERMANAGER_H_

#endif /* TIMERMANAGER_H_ */
