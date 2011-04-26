/**
\file ConsensusAverage.h

ConsensusAverage.h provides a distributed average consensus algorithm. The algorithm implemented corrosponds almost direclty to the A2 algorithm proposed in: M. Mehyar et al. (2007) "Asynchronous Distributed Averaging on Communication Networks"

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
\todo document code
\note This software is distributed under the BSD open-source license.
*/

#ifndef CONSENSUS_AVERAGE_H_

#include <ase/config/ConfigASE.h>

typedef struct {
	short label;
	float gamma; /*between 0 and 1/(degree+1)*/
	float phi; /*between 0 and 0.5*/
	float delta[MAX_NODE_DEGREE];
	float xNeighbors[MAX_NODE_DEGREE];
	float x;
	float z;
} consensus_t;

typedef struct {
	char msgType;
	short from_label;
	char reply;
	float value;
} consensusMsg_t;

void consensus_init(consensus_t *consensus);
void consensus_update(consensus_t *consensus);
void consensus_handleMessage(consensus_t *consensus, consensusMsg_t *msg, int channel);
void consensus_setValue(float newValue, consensus_t *consensus);


#define CONSENSUS_AVERAGE_H_

#endif /* CONSENSUS_AVERAGE_H_ */

