/**
 * \file RandomSearch.h
 * This file provides an implementation of the random search
 *
 * \date January 2010
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef RANDOMSEARCH_H_
#include <ase/config/ConfigASE.h>
#include <ase/tools/RewardCollector/RewardCollector.h>
#include <ase/communication/Message.h>
#include <stdint.h>

/**
 * \extends Message_t
 */
typedef struct {
	Message_t super;
	uint8_t stateCount;
	float reward;
} RandomSearchMsg_t;

/**
 * Contains the parameters and variables (theta) of the SPSA learning algorithm
 */
typedef struct {
	short label;
	float theta[SPSA_N_PARAMETERS_MAX];
	float maxTheta[SPSA_N_PARAMETERS_MAX];
	float maxReward;
	int nItt;
	int maxItt;
	int nParameters;
	rewardCollector_t collector;
} RandomSearch_t;

/**
 * Initialize the search process with default parameters
 */
void RandomSearch_init(RandomSearch_t* process);

/**
 * Reset the variables to a random value between (0 to 1)
 */
void RandomSearch_reset(RandomSearch_t* process);

/**
 * Update variables based on previously received reward and reset the reward to zero
 */
void RandomSearch_update(RandomSearch_t* process);

/**
 * Reset reward to zero without changing the variables
 */
void RandomSearch_ignoreUpdate(RandomSearch_t* process);

/**
 * If the random search process is ready to be updated.
 * The condition is that it must have received a reward since the last update.
 */
int RandomSearch_isReady(RandomSearch_t* process);

/**
 * Get the reward received by the search process.
 */
float RandomSearch_getReward(RandomSearch_t* process);

float RandomSearch_getMaxReward(RandomSearch_t* process);

int RandomSearch_getMaxIndex(RandomSearch_t* process);
/**
 * Set the reward (typically based on local sensor information or a reward message
 * send from an external supervisor.
 */
void RandomSearch_collectReward(RandomSearch_t* process, float reward);

/**
 * Get the found variables with the highest reward
 */
float RandomSearch_getMaxTheta(RandomSearch_t* process, int index);

/**
 * Get a random variable
 */
float RandomSearch_getTheta(RandomSearch_t* process, int index);

/**
 * Set the learning variables from an array of float (must have length equal to SPSA_N_PARAMETERS_MAX).
 */
void RandomSearch_setTheta(RandomSearch_t* process, float* variable);

#define RANDOMSEARCH_H_

#endif
