/**
 * \file RandomSearchCpgGait.h
 *
 * Created on: Jan 7, 2010
 *
 * \author David Johan Christensen
 */

#ifndef RANDOMSEARCHCPGGAIT_H_

/**
 * Initialize the gait with default parameters
 */
void RandomSearchCpgGait_init();

/**
 * When a new reward message has been received it will update the gait.
 * Must be called often.
 */
int RandomSearchCpgGait_act();

/**
 * Handles messages of the type LEARNING_MESSAGE, which contains a rewards signal (any)
 */
void RandomSearchCpgGait_handleMessage(char* message, char messageSize, char channel);
/**
 * Force update of gait e.g. after changing parameters
 */
void RandomSearchCpgGait_updateGait();

/**
 * Exploit state makes the robot move with be so far best found gait
 */
void RandomSearchCpgGait_setStateExploit();

/**
 * The robot will be exploring its gait parameters to optimize its gait based on
 * received reward.
 */
void RandomSearchCpgGait_setStateLearning();

/**
 * Make the module go to its home position and stop
 */
void RandomSearchCpgGait_setStateHome();

void RandomSearchCpgGait_reset();

void RandomSearchCpgGait_logCurrentController(int id, FILE* log);
void RandomSearchCpgGait_logBestControlInfo(int id, FILE* log);

#define RANDOMSEARCHCPGGAIT_H_

#endif /* RANDOMSEARCHCPGGAIT_H_ */
