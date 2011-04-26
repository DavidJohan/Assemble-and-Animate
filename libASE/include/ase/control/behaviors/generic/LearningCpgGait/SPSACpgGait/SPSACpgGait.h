/**
 * \file SPSACpgGait.h
 * This file provides a generic online learning system of gaits for any actuated modular robot.
 * It is based on CpgGait.h which is based on Hopf Cpg's. It uses the SPSA stochastic optimization
 * algorithm to optimize the gait parameters such as amplitude and phase difference.
 *
 *
 * Created on: July 5, 2009
 *
 * \see SPSALearning.h
 * \see CpgGait.h
 * \see HopfCartesianCpg.h
 * \author David Johan Christensen
 */

#ifndef SPSACPGGAIT_H_
/**
 * Initialize the gait with default parameters
 */
void SPSACpgGait_init();

/**
 * When a new reward message has been received it will update the gait.
 * Must be called often.
 */
int SPSACpgGait_act();

/**
 * Resets the underlying SPSA parameter optimization (forget all) either
 * with initial conditions randomized or at the home position
 */
void SPSACpgGait_reset(int randInit);

/**
 * Handles messages of the type LEARNING_MESSAGE, which contains a rewards signal (any)
 */
void SPSACpgGait_handleMessage(char* message, char messageSize, char channel);
/**
 * Force update of gait e.g. after changing parameters
 */
void SPSACpgGait_updateGait();

/**
 * Set learning parameters, ak and ck, used in the SPSA learning algorithm
 */
void SPSACpgGait_setLearningParameters(float ak_new, float ck_new);


/**
 * Exploit state makes the robot move with be so far best found gait
 */
void SPSACpgGait_setStateExploit();

/**
 * The robot will be exploring its gait parameters to optimize its gait based on
 * received reward.
 */
void SPSACpgGait_setStateLearning();

/**
 * Make the module go to its home position and stop
 */
void SPSACpgGait_setStateHome();

/**
 * Makes the robot change to another random gait each time a reward message is received.
 */
void SPSACpgGait_setStateRandom();

/**
 * Set theta from current gait in underlying CpgGait
 */
void SPSACpgGait_setThetaFromCpgGait();

/**
 * Write learning parameters ak and ck to a given log file.
 */
void SPSACpgGait_logLearningParameters(int id, FILE* log);
void SPSACpgGait_logController(int id, FILE* log);

#define SPSACPGGAIT_H_

#endif /* SPSACPGGAIT */
