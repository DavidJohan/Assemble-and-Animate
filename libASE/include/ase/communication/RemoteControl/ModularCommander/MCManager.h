/**
 * \file MCManager.h
 *
 * MCManager.h provides an way to communication with the remote control application
 * ModularCommander.
 *
 *  Created on: March 8, 2009
 *  Author: David Johan Christensen
 */

#ifndef MCMANAGER_H_
#include <stdint.h>
#include <ase/config/ConfigASE.h>

/**
 * If receiver ID, recID, is set to MC_BROADCAST, the message will be
 * broadcasted to all other modules in the robot.
 */
#define MC_BROADCAST	255

/**
 * If receiver ID, recID, is set to MC_WILDCARD, the message will be received by any module
 * but will not be transmitted further to other modules.
 */
#define MC_WILDCARD		254


/**
 * A container for messages used by ModularCommander.
 * - msgType must be MODULAR_COMMANDER_MESSAGE
 * - msgID is incremented for each message send
 * - recID it the ID of the module/modules that should received the message
 * - mcType is the type of command equivalent to the workings of ModularCommander
 */
typedef struct {
	uint8_t msgType;
	uint8_t msgID;
	uint8_t recID;
	uint8_t mcType;
	uint8_t payload[MC_MAX_PAYLOAD];
} MCManager_msg;

typedef struct {
	uint8_t isUnhandled;
	uint8_t payloadLength;
	uint8_t channel;
	MCManager_msg msg;
} MCManager_command;

typedef struct {
	uint8_t isUsed;
	uint8_t mcType;
	void (*handler)(uint8_t, uint8_t*, uint8_t, uint8_t);
} MCManager_handler;


/**
 * Initialize the Modular Commander Manager.
 */
void MCManager_init();


/**
 * Set the ID used by Modular Commander in this module.
 */
void MCManager_setID(uint8_t ID);

int MCManager_getID();

/**
 * Install a handler (call-back function) which will subscribe to events generated by a message
 * being received on the module. The handler should take the following arguments:
 * - handler(uint8_t mcType, uint8_t* payload, uint8_t payloadLength, uint8_t channel);
 */
void MCManager_installHandler(uint8_t mcType, void (handler)(uint8_t, uint8_t*, uint8_t, uint8_t));


#define MCMANAGER_H_

#endif /* MCMANAGER */
