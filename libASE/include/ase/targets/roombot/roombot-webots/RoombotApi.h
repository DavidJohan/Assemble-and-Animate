/**
 * \file RoombotApi.h
 *  This is an Webots based API for Roombot which should be
 *  transferable to the real modules. It abstracts away Webot specific details and provides
 *  a clean C interface to controlling a module.
 *
 * The following notation is used for mapping the Webot tags to integer IDs:
 * - Connectors have index 0...9 (starting from C0X,COY,COZ,C1X,...,C3Z)
 * - IR communication channels index: 0...9 and corrosponds to the connectors
 * - Radio communication channel have index 10
 * - Actuators have index s1=0, m1=1, s2=2
 *
 * Further it is assumed that the modules name in webots is on the form "module_X" where X is a
 * unique hardware ID of the module.
 *
 * \see AbstractModuleApi.h
 *
 *  Created on: July, 2009
 *
 *  \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 *  \note This software is distributed under the BSD open-source license.
 */

#ifndef ROOMBOTAPI_H_
#define ROOMBOTAPI_H_
#include <webots/robot.h>
/**
 * Initilizes the API
 */
void roombotApi_init(void (*handler)(char*,char,char));
/**
 * Connect connector with ID connectorID
 */
void roombotApi_connect(int connectorID);
/**
 * Disconnect connector with ID connectorID
 */
void roombotApi_disconnect(int connectorID);
/**
 * If the module is connected on a specific connector
 */
int roombotApi_isConnected(int connectorID);
/**
 * If another connnector on another module is nearby the specific connector
 */
int roombotApi_getConnectorPresence(int connectorID);

/**
 * Get position of the actuator with id [0,1,2]
 */
double roombotApi_getServoPosition(int servoID);

/**
 * Set the position that the underlying p-controller will move the actuator towards
 */
void roombotApi_setServoPosition(int servoID, double goal);

/**
 * Send a message on a given communication channel IR(0-9) Wifi (10)
 */
void roombotApi_sendMessage(char* msg, int length, int channel);
/**
 * Send the same message to all neighbor channels IR (0-9)
 */
void roombotApi_sendMessageToAll(char* msg, int length);

/**
 * Set the callback function which will be called each time a message is received
 */
void roombotApi_setMessageHandler(void (*handler)(char*,char,char));

/**
 * Checks for new messages, must be called often (every step)
 */
int roombotApi_checkForEvents();

/**
 * Get the time (in seconds) since the module were started.
 * Not will be different on different modules (at least in hardware)
 */
double roombotApi_getLocalTime();


/**
 * Get the time (in milli-seconds) since the module were started.
 * Not will be different on different modules (at least in hardware)
 */
long roombotApi_getLocalMsTime();

/**
 * Get a random seed which is different on each module
 */
long roombotApi_getRandomSeed();

/**
 * Get the hardware ID of the module
 */
int roombotApi_getID();

/**
 * Run a test program, testing this API
 */
void roombotApi_runTestProgram();

/**
 * Get an estimate of the energy consumed by actuators since the module
 * were started.
 * Disclamer: very uncertain
 */
float roombotApi_getEnergyConsumed();

/**
 * Get acceleration on a given axis
 */
double roombotApi_getAccelerometerValue(int axis);

#endif /* ROOMBOTAPI_H_ */
