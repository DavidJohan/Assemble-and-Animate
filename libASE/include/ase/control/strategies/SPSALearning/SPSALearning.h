/**
 * \file SPSALearning.h
 * This file provides an implementation of the simultaneous perturbation stochastic
 * approximation (SPSA) method (by James C. Spall). It can be used for any optimization
 * until now it has been used for online optimization of locomotion.
 *
 * A number of variables, called theta, (scaled in the interval 0-1) are optimized based on a reward signal
 *
 * \see SPSACpgGait.h
 * \date July 2009
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef SPSALEARNING_H_
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
} SPSALearningMsg_t;

/**
 * \extends Message_t
 */
typedef struct {
	Message_t super;
	uint8_t stateCount;
	uint8_t reward;
} SPSALearningMsgByte_t;

/**
 * Contains the parameters and variables (theta) of the SPSA learning algorithm
 */
typedef struct {
	short label;
	float ak;
	float ck;
	float theta[SPSA_N_PARAMETERS_MAX];
	float gradient[SPSA_N_PARAMETERS_MAX];
	int delta[SPSA_N_PARAMETERS_MAX];
	float yPlus;
	float yMinus;
	float yPlus_old;
	float yMinus_old;
	float yEMA;
	int state;
	float deltaTime;
	float nextUpdateTime;
	int nParameters;
	rewardCollector_t collector;
} SPSALearning_t;

/**
 * Initialize the learning process with default parameters
 */
void SPSALearning_init(SPSALearning_t* process);

/**
 * Reset the variables (theta) to a random value between (0 to 1) if random = 1
 * otherwise to to 0.5
 */
void SPSALearning_reset(SPSALearning_t* process, int random);

/**
 * Update variables (theta) in the learning process based on previously received reward
 * and reset the reward to zero
 */
void SPSALearning_update(SPSALearning_t* process);

/**
 * Reset reward to zero without changing the variables
 */
void SPSALearning_ignoreUpdate(SPSALearning_t* process);

/**
 * If the learning process is ready to be updated.
 * The condition is that it must have received a reward since the last update.
 */
int SPSALearning_isReady(SPSALearning_t* process);

/**
 * Get the reward received by the learning process.
 */
float SPSALearning_getReward(SPSALearning_t* process);

/**
 * Set the reward (typically based on local sensor information or a reward message
 * send from an external supervisor.
 *
 */
void SPSALearning_collectReward(SPSALearning_t* process, float reward);
/**
 * Get the learning variables (theta) without perturbing them (estimation of optimal value so far)
 */
float SPSALearning_getThetaNonPerturbed(SPSALearning_t* process, int index);

/**
 * Get the learning variables (theta) with perturbation (new parameters to explore).
 */
float SPSALearning_getTheta(SPSALearning_t* process, int index, int wrap);

/**
 * Set the learning variables from an array of float (must have length equal to SPSA_N_PARAMETERS_MAX).
 */
void SPSALearning_setTheta(SPSALearning_t* process, float* theta);

#define SPSALEARNING_H_

#endif /* SPSALEARNING_H_ */
