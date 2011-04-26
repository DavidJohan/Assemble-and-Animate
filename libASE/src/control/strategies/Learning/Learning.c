#include <stdlib.h>
#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/Learning/Learning.h>
#include <ase/tools/RewardCollector/RewardCollector.h>


void learning_init(Learning_t* process) {
	process->label = 1;
	process->alpha = 0.1;
	process->epsilon = 0.8;
	process->nActions = 3;
	process->accelerated = 1;
	process->accelerateNow = 0;
	process->currentAction = 0;
	learning_reset(process);
	rewardCollector_init(&(process->collector), REWARDCOLLECTOR_LATEST);
}

void learning_reset(Learning_t* process) {
	int i;
	for(i=0;i<process->nActions;i++) {
		process->rewardEstimation[i] = 99;
	}
}


void learning_collectReward(Learning_t* process, float reward) {
	rewardCollector_collect(&process->collector, reward);
}

int learning_isReady(Learning_t* process) {
	return rewardCollector_isUpdated(&process->collector);
}

int learning_startup(Learning_t* process) {
	int i;
	for(i=0;i<process->nActions;i++) {
		if(process->rewardEstimation[i]== 99) {
			return 1;
		}
	}
	return 0;
}

void learning_update(Learning_t* process) {
	if(!learning_isReady(process)) {
		#ifdef PRINTF
		ase_printf("Warning: #1 in Learning \n"); //  learning process not updated since no reward value received
		#endif
		return;
	}
	float reward = rewardCollector_getReward(&process->collector);
	rewardCollector_resetReward(&process->collector);
	if(process->rewardEstimation[process->currentAction]==99) { //in start phase
		process->rewardEstimation[process->currentAction] = reward;
	}
	float estimation = process->rewardEstimation[process->currentAction];
	if(process->accelerated && reward > estimation ) {
		process->accelerateNow = 1;
	}
	process->rewardEstimation[process->currentAction] += process->alpha*(reward-estimation);
}




int learning_accelerateNow(Learning_t* process) {
	return process->accelerateNow;
}

float learning_getReward(Learning_t* process) {
	return rewardCollector_getReward(&process->collector);
}

int learning_getAction(Learning_t* process) {
	if(process->accelerateNow) {
		process->accelerateNow = 0;
	}
	else {
		if(rand()%100<100*process->epsilon&&!learning_startup(process)) {
			process->currentAction = learning_getBestAction(process);
		}
		else {
			process->currentAction = rand()%process->nActions;
		}
	}
	return process->currentAction;
}


int learning_getRandomMaxAction(float max, Learning_t* process) {
	int deadLockCount = 1000;
	int index;
	while(deadLockCount>0) {
		index = rand()%process->nActions;
		if(process->rewardEstimation[index] == max) {
			return index;
		}
		deadLockCount--;
	}
	#ifdef PRINTF
	ase_printf("Warning: #2 in Learning \n"); //  deadlock detected in learning_getRandomMaxAction
	#endif
	return 0;
}

int learning_getBestAction(Learning_t* process) {
	int best = 0, notOneMax = 0, i;
	float max = process->rewardEstimation[best];
	for(i=0;i<process->nActions;i++) {
		if(max<process->rewardEstimation[i]) {
			best = i;
			max = process->rewardEstimation[best];
			notOneMax = 0;
		}
		else if(max==process->rewardEstimation[i]) {
			notOneMax = 1;
		}
	}
	if(notOneMax) {
		return learning_getRandomMaxAction(max, process);
	}
	return best;
}
