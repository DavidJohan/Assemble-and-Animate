/**
 * \file DistributedStateManager.h
 * DistributedStateManager.h provides a distributed state machine with pending states.
 * It is used by the "dynarole" language to perform self-reconfiguration.
 *
 * \see DistributedStateMerger.h
 *
 * \date March 11, 2009
 * \author Ulrik P. Shultz
 * \author Mirko Bordignon
 * \author David Johan Christensen
 */

#ifndef DSMANAGER_H_
#include <stdint.h>
#include <stdbool.h>
#include <ase/config/ConfigASE.h>

typedef struct {
	char msgType;
	char state;
	char recipientId;
	char alternateSequenceFlag;
	char pendingStates[DSM_MAX_PENDING_STATES];
} DSManager_msg;

//TODO should extend PeriodicStrategy
typedef struct {
	float deltaTime;
	float nextUpdateTime;
} DSManager_update_t;


void DSManager_init(int id);

int DSManager_act();


/**
 * Send a certain state for a certain recipient
 */
void DSManager_sendState(uint8_t state, uint8_t recipientId);

/**
 * Add a state to the pending states vector
 */
void DSManager_addPendingState(uint8_t state);

/**
 * Remove a state from the pending states vector
 */
void DSManager_removePendingState(uint8_t state);

/**
 * Check whether there is any currently pending state
 */
bool DSManager_pendingStatesPresent();

uint8_t DSManager_getMyNewState();
uint8_t DSManager_getGlobalState();

void DSManager_reset_state();
void DSManager_reset_sequence();
void DSManager_reset();

#define DSMANAGER_H_

#endif /* DSMANAGER */
