/**
\file RewardCollector.h

RewardCollector.h collects (sum or relace type) the reward as needed by the basic learning strategy.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef REWARDCOLLECTOR_H_


#define REWARDCOLLECTOR_SUM	1
#define REWARDCOLLECTOR_MAX	2
#define REWARDCOLLECTOR_MIN	3
#define REWARDCOLLECTOR_LATEST	4

typedef struct {
	float reward;
	int type;
	int updateCount;
} rewardCollector_t;

void rewardCollector_init(rewardCollector_t* collector, int type);
int rewardCollector_isUpdated(rewardCollector_t* collector);
void rewardCollector_collect(rewardCollector_t* collector, float reward);
float rewardCollector_getReward(rewardCollector_t* collector);
void rewardCollector_resetReward(rewardCollector_t* collector);

#define REWARDCOLLECTOR_H_

#endif /* REWARDCOLLECTOR_H_ */


