/**
\file Learning.h

Learning.h a simple reinforcement learning algoritm for modular robots. Following the algorithm described in:
D.J.Christensen et al. "Morphology Independent Learning for Modular robots" DARS2008
Basically each module is independently optimizing it own reward using an exponential moving average.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef LEARNING_H_
#include <ase/communication/Message.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/RewardCollector/RewardCollector.h>


/**
 *
 * \extends Message_t
 */
typedef struct {
	Message_t super;
	char stateCount;
	float reward;
} LearningMsg_t;

typedef struct {
	short label;
	float alpha;
	float epsilon;
	float deltaTime;
	float nextUpdateTime;
	int accelerated;
	int nActions;
	int currentAction;
	int accelerateNow;
	float rewardEstimation[LEARNING_MAX_ACTIONS];
	rewardCollector_t collector;
} Learning_t;


void learning_init(Learning_t* process);
void learning_reset(Learning_t* process);
void learning_update(Learning_t* process);
int learning_accelerateNow(Learning_t* process);
int learning_getAction(Learning_t* process);
float learning_getReward(Learning_t* process);
int learning_isReady(Learning_t* process);
int learning_getBestAction(Learning_t* process);
void learning_collectReward(Learning_t* process, float reward);

#define LEARNING_H_

#endif /* LEARNING_H_ */
