/**
 * \file CommDefs.h
 * CommDefs.h defines the types of communication messages that are recognized and used internally by ASE
 * the convention is that the first byte in a message defines its type, e.g., a CPG_MESSAGE will
 * have a 2 as its first byte. This information is needed for internal routing of messages.
 * Note that application specific messages should avoid using the same types as defined here.
 *
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */


#define PING_MESSAGE				1
#define CPG_CNO_MESSAGE				2
#define GRADIENT_MESSAGE			3
#define LEARNING_MESSAGE			4
#define CONSENSUS_MESSAGE			5
#define MODULAR_COMMANDER_MESSAGE	6
#define DISTRIBUTED_STATE_MESSAGE	7
#define GOSSIP_MESSAGE				8
#define CPG_HC_MESSAGE				9
#define INTERNAL_MESSAGE			254		//messages send between components through
#define BROADCAST_MESSAGE			255
