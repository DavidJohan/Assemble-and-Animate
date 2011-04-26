#include <ase/control/strategies/Consensus/ConsensusAverage.h>
#include <math.h>
#include <stdio.h>
#include <ase/communication/CommDefs.h>
#include <ase/targets/AbstractModuleApi.h>

consensusMsg_t consensusMsg;

int msgSend=0;
int msgGot=0;
void consensus_init(consensus_t *consensus) {
	consensus->label = 1;
	consensus->gamma = 0.1; //0.1 /*between 0 and 1/(degree+1)*/
	consensus->phi = 0.05; //0.05	/*between 0 and 0.5*/
	consensus->x = 0;
	consensus->z = 0;
	int i;
	for (i=0;i<MAX_NODE_DEGREE;i++) {
		consensus->delta[i] = 0;
		consensus->xNeighbors[i] = 0;
	}
}


/**
* Every node i sends to every neighbor a STATE message
* that contains its current state value xi from time to time.
* Each node also, from time to time, executes the update
* rule (first equation in (13)) with the information it has
* about other state values.
*/
void consensus_MR1P1(consensus_t *consensus) {
	float deltaSum = 0;
	int i;
	for(i=0;i<getNumberOfNeighborChannels();i++) {
		if(hasNeighbor(i)==1) {
			deltaSum += consensus->delta[i];
		}
	}
	deltaSum+= consensus->z-consensus->x;
	consensus->x = consensus->x + consensus->gamma * deltaSum;
}

void consensus_MR1P2(consensus_t *consensus) {
	consensusMsg.msgType = CONSENSUS_MESSAGE;
	consensusMsg.from_label = consensus->label;
	consensusMsg.reply = 0;
	consensusMsg.value = consensus->x;
	int nMsg = sendMessageToAllNeighbors((char*)&consensusMsg, sizeof(consensusMsg));
	msgSend+=nMsg;
}

/**
* On link ij, if delta_ij is the sender, it executes the update rule
* (second equation in (13)) from time to time. Whenever
* delta_ij executes the update rule, it also sends to its receiver
* delta_ji a REPLY message that contains the value of the
* change it has made in the value of delta_ij. delta_ij will not
* execute the update rule again until the TCP ACK of
* this REPLY message comes back.
*/
void consensus_MR2(consensus_t *consensus) {
	int i;
	for(i=0;i<getNumberOfNeighborChannels();i++) {
		if(hasNeighbor(i)==1) {
			float diff = consensus->phi*(consensus->xNeighbors[i]-consensus->x); //problem: what if more than one neighbor per channel?
			consensus->delta[i] += diff;

			consensusMsg.msgType = CONSENSUS_MESSAGE;
			consensusMsg.from_label = consensus->label;
			consensusMsg.reply = 1;
			consensusMsg.value = diff;
			int success = sendMessageToNeighbor((char*)&consensusMsg, sizeof(consensusMsg), i); //should wait for reply (but I will not?)
			if(success!=1) {
				#ifdef PRINTF
				ase_printf("Warning: #1 in ConsensusAverage\n"); //the message was not send
				#endif
			}
			else {
				msgSend++;
			}
 		}
	}
}
/**
* If delta_ji is the receiver on link ij, it waits for REPLY
* messages from delta_ij and subtracts the value in the
* message from the value of delta_ji. (Note that the REPLY
* message does not directly change the value of x_ij.)
*/
void consensus_MR3(consensus_t *consensus, consensusMsg_t *msg, int channel) {
	if(msg->from_label==consensus->label) {
		if(msg->reply==1) {
			consensus->delta[channel] -= msg->value; //Semaphor needed here?
		}
		else {
			consensus->xNeighbors[channel] = msg->value; //save for later use in updates
		}
		msgGot++;
	}
}

void consensus_update(consensus_t *consensus) {
	consensus_MR1P1(consensus); //update x
	consensus_MR1P2(consensus); //send state
	consensus_MR2(consensus); //update delta and send reply
}

void consensus_handleMessage(consensus_t *consensus, consensusMsg_t *msg, int channel) {
	consensus_MR3(consensus, msg, channel); //handle message (reply or state)
}


void consensus_setValue(float newValue, consensus_t *consensus) {
	float diff = newValue - consensus->z;
	consensus->z += diff;
	consensus->x += diff;
}

