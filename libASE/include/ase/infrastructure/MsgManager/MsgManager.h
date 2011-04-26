/**
\file MsgManager.h

MsgManager.h provides a publish/subscribe system of messages for components

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
\note This software is distributed under the BSD open-source license.
*/

#ifndef MSGMANAGER_H_
#define MSGMANAGER_H_
#include <ase/communication/Message.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/infrastructure/Entity/Manager/PubSubManager/PubSubManager.h>
#include <ase/tools/LinkedList/LinkedList.h>

typedef struct {
	char msgType;
	void (*handler)(Msg_t*);
} MsgHandler_t;


/**
 * A manager of messages
 *
 *\extends PubSubManager_t
 */
typedef struct {
	PubSubManager_t super;
} MsgManager_t;


/**
 * Allows a component to subscribe to specific message types
 */
void MsgManager_subscribe(char msgType, void (*handler)(Msg_t*));

#endif /* MSGMANAGER_H_ */


