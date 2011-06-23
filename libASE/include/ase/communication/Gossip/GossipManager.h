/**
\file GossipManager.h

GossipManager.h enables the modules to share data in an epedemic fashion.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark
\todo Document code
\note This software is distributed under the BSD open-source license.
*/

#ifndef GOSSIPMANAGER_H_
#include <stdint.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/Timer/TimerManager.h>

typedef struct {
	uint8_t msgType;
	uint8_t label;
	char payload[GOSSIP_MAX_MSG_SIZE];
} gossip_msg;

typedef struct {
	uint8_t isNew;
	gossip_msg msg;
	char msgSize;
	char msgChannel;
} gossip_heard;

struct GOSSIP_TAG {
	uint8_t isUsed;
	uint8_t label;
	long msDelay;
	long msVariance;
	Timer_t* timer;
	gossip_msg msg;
	char messageSize;
	gossip_heard heard;
	void (*handler)(char*, char, char);
	void (*updater)(struct GOSSIP_TAG*);
};
typedef struct GOSSIP_TAG gossip_t;


void GossipManager_init();
gossip_t* GossipManager_createGossip(uint8_t label, long msDelay, long msVariance, void (handler)(char*, char, char), void (*updater)(gossip_t*));
void GossipManager_updateGossip(gossip_t* gossip, char* message, char messageSize);
int GossipManager_remove(gossip_t* gossip);
int GossipManager_start(gossip_t* gossip);
int GossipManager_pause(gossip_t* gossip);

#define GOSSIPMANAGER_H_
#endif /* GOSSIPMANAGER_H_ */

