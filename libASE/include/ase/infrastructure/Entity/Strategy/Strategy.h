/**
\file Strategy.h
Strategy.h is a top-level construct for ASE. Most strategies extend it. It contains methods and variables
to start, stop, pause, resume and restart the strategy.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maersk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef STRATEGY_H_
#define STRATEGY_H_
#include <stdbool.h>
#include <ase/infrastructure/Entity/Entity.h>
/**
 * This struct contains the variables and methods to start, stop,
 * pause, resume and restart the Strategy.
 * \extends Entity_t
 */
typedef struct {
	Entity_t super;
	bool started:1;
	bool paused:1;
	void (*stop)(void*);
	void (*start)(void*);
	void (*pause)(void*);
	void (*resume)(void*);
	void (*restart)(void*);
} Strategy_t;

/**
 * Initialize the Strategy
 */
void Strategy_init(Strategy_t* strategy, short label);

/**
 * Stop the Strategy
 */
void Strategy_stop(Strategy_t* strategy, void* data);

/**
 * Start the Strategy
 */
void Strategy_start(Strategy_t* strategy, void* data);

/**
 * Pause the Strategy
 */
void Strategy_pause(Strategy_t* strategy, void* data);

/**
 * Resume the Strategy
 */
void Strategy_resume(Strategy_t* strategy, void* data);

/**
 * Restart the Strategy
 */
void Strategy_restart(Strategy_t* strategy, void* data);

/**
 * Is the Strategy initialized?
 */
bool Strategy_isInitialized(Strategy_t* strategy);

/**
 * Is the Strategy started or stopped?
 */
bool Strategy_isStarted(Strategy_t* strategy);

/**
 * Is the Strategy paused or running?
 */
bool Strategy_isPaused(Strategy_t* strategy);

/**
 * Set the handler that is called when strategy is started
 */
void Strategy_setStartHandler(Strategy_t* strategy, void (*start)(void*));

/**
 * Set the handler that is called when strategy is paused
 */
void Strategy_setPauseHandler(Strategy_t* strategy, void (*pause)(void*));

/**
 * Set the handler that is called when strategy is resumed
 */
void Strategy_setResumeHandler(Strategy_t* strategy, void (*resume)(void*));

/**
 * Set the handler that is called when strategy is stopped
 */
void Strategy_setStopHandler(Strategy_t* strategy, void (*stop)(void*));

/**
 * Set the handler that is called when strategy is restarted
 */
void Strategy_setRestartHandler(Strategy_t* strategy, void (*restart)(void*));


#endif /* STRATEGY_H_*/
