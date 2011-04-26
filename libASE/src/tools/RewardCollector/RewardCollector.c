#include <stdio.h>
#include <stdlib.h>
#include <ase/tools/RewardCollector/RewardCollector.h>
#include <ase/communication/CommDefs.h>


void rewardCollector_init(rewardCollector_t* collector, int type) {
	collector->reward = 0;
	collector->updateCount = 0;
	collector->type = type;
}

int rewardCollector_isUpdated(rewardCollector_t* collector) {
	return collector->updateCount !=0;
}

void rewardCollector_collect(rewardCollector_t* collector, float reward) {
	if(collector->type==REWARDCOLLECTOR_SUM) {
		collector->reward += reward;
	}
	else if(collector->type==REWARDCOLLECTOR_MAX) {
		if(collector->reward<reward) {
			collector->reward = reward;
		}
	}
	else if(collector->type==REWARDCOLLECTOR_MIN) {
		if(collector->reward>reward) {
			collector->reward = reward;
		}
	}
	else if(collector->type==REWARDCOLLECTOR_LATEST) {
		collector->reward = reward;
	}
	collector->updateCount++;
}

float rewardCollector_getReward(rewardCollector_t* collector) {
	float reward = collector->reward;
	return reward;
}
void rewardCollector_resetReward(rewardCollector_t* collector) {
	collector->updateCount = 0;
	collector->reward = 0;
}
