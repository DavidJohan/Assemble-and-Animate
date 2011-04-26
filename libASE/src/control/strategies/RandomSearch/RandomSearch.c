#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/RandomSearch/RandomSearch.h>
#include <ase/tools/RewardCollector/RewardCollector.h>

void RandomSearch_init(RandomSearch_t* process) {
	process->label = 1;
	process->nParameters=SPSA_N_PARAMETERS_MAX;
	process->nItt = 0;
	process->maxItt = 0;
	RandomSearch_reset(process);
	rewardCollector_init(&(process->collector), REWARDCOLLECTOR_LATEST);
}

static void randomizeTheta(RandomSearch_t* process) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		process->theta[i] = ((float)rand())/(((float)RAND_MAX)+0.1f); //initialize a random solution
	}
}

void RandomSearch_reset(RandomSearch_t* process) {
	randomizeTheta(process);
	int i;
	for(i=0;i<process->nParameters;i++) {
		process->maxTheta[i] = -1.0f;
	}
	process->maxReward = -FLT_MAX;
}

void RandomSearch_update(RandomSearch_t* process) {
	if(!RandomSearch_isReady(process)) {
		#ifdef PRINTF
		ase_printf("Warning: #1 in RandomSearch \n"); //  learning process not updated since no reward value received
		#endif
		return;
	}
	float reward = rewardCollector_getReward(&process->collector);
	rewardCollector_resetReward(&process->collector);

	if(reward>process->maxReward) {
		process->maxReward = reward;
		process->maxItt = process->nItt;
		ase_printf("Found new max reward %f\n",process->maxReward);
		int i;
		for(i=0;i<process->nParameters;i++) {
			process->maxTheta[i] = process->theta[i];
		}
	}
	randomizeTheta(process);
	process->nItt++;
}

void RandomSearch_ignoreUpdate(RandomSearch_t* process) {
	rewardCollector_resetReward(&process->collector);
}

float RandomSearch_getMaxTheta(RandomSearch_t* process, int index) {
	return process->maxTheta[index];
}

int RandomSearch_getMaxIndex(RandomSearch_t* process) {
	return process->maxItt;
}

float RandomSearch_getTheta(RandomSearch_t* process, int index) {
	return process->theta[index];
}

void RandomSearch_setTheta(RandomSearch_t* process, float* theta) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		process->theta[i] = theta[i];
	}
}

float RandomSearch_getReward(RandomSearch_t* process) {
	return rewardCollector_getReward(&process->collector);
}

float RandomSearch_getMaxReward(RandomSearch_t* process) {
	return process->maxReward;
}

void RandomSearch_collectReward(RandomSearch_t* process, float reward) {
	rewardCollector_collect(&process->collector, reward);
}

int RandomSearch_isReady(RandomSearch_t* process) {
	return rewardCollector_isUpdated(&process->collector);
}
