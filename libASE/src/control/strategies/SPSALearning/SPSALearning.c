#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/tools/RewardCollector/RewardCollector.h>


static float clamp(float val) {
	if(val>1) return 1;
	else if(val<0) return 0;
	else return val;
}
static float wrapClamp(float val) {
	if(val>1) return clamp(val-1);
	else if(val<0) return clamp(val+1);
	else return val;
}
static float max(float x1, float x2) {
	if(x1>x2) return x1;
	else return x2;
}
static float min(float x1, float x2) {
	if(x1>x2) return x2;
	else return x1;
}
static int isBlocked(SPSALearning_t* process) {
	int type=-1;
	if(type==1) {
		float sumThen = process->yMinus_old + process->yPlus_old;
		float sumNow = process->yMinus + process->yPlus;
		float improvement = sumNow/sumThen;
		if(improvement<1) {
			#ifdef PRINTF
			ase_printf("Blocked since improvement is only %f \n",improvement);
			#endif
			return 1;
		}
	}
	if(type==3) {
		float maxThen = max(process->yMinus_old,process->yPlus_old);
		float maxNow = max(process->yMinus,process->yPlus);
		float improvement = maxNow-maxThen;
		if(improvement<0) {
			#ifdef PRINTF
			ase_printf("Blocked since improvement is negative %f \n",improvement);
			#endif
			return 1;
		}
	}
	if(type==4) {
		float minThen = min(process->yMinus_old,process->yPlus_old);
		float minNow = min(process->yMinus,process->yPlus);
		float improvement = minNow-minThen;
		if(improvement<0) {
			#ifdef PRINTF
			ase_printf("Blocked since improvement is negative %f \n",improvement);
			#endif
			return 1;
		}
	}
	if(type==5) {
		float yk = process->yMinus + process->yPlus;
		float improvement = yk/process->yEMA;
		process->yEMA = 0.25*yk+(1-0.25)*process->yEMA;
		if(improvement<1.0) {
			#ifdef PRINTF
			ase_printf("Blocked since improvement is less than 1 (%f) \n",improvement);
			#endif
			return 1;
		}
	}
	return 0;
}
static void updateTheta(SPSALearning_t* process) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		float ak = process->ak;
		float ck = process->ck;
		float theta = process->theta[i];
		float delta = process->delta[i];
		float yPlus = process->yPlus;
		float yMinus = process->yMinus;
		float g = (yMinus-yPlus)/(2*ck*delta); //here is the magic!
		int blocked = isBlocked(process);
		if(0) {
			float p= 0.5;
			process->gradient[i] = p*process->gradient[i] +(1-p)*g;
			process->theta[i] = theta - ak*process->gradient[i];
		}
		else {
			float step = ak*g;
			float maxStep = 0.05f;
			if(fabs(step)>maxStep) {
				//float temp=step;
				step = ((step>0)?1:-1)*maxStep;
				//printf("Outlier protection at work (%f -> %f)\n",temp, step);
			}
			if(!blocked) {
				process->theta[i] = theta - step;
			}
			else {
				printf("Update blocked\n");
			}
			process->yPlus_old = process->yPlus;
			process->yMinus_old = process->yMinus;
		}
		process->theta[i] = clamp(process->theta[i]);
	}
}

static void randomizeDelta(SPSALearning_t* process) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		process->delta[i] = (rand()%2==0)?-1:1; //50% probability of -1 or 1
	}
}

void SPSALearning_init(SPSALearning_t* process) {
	process->label = 1;
	process->ak = 0.001f;
	process->ck = 0.001f;
	process->nParameters=SPSA_N_PARAMETERS_MAX;
	SPSALearning_reset(process,0);
	rewardCollector_init(&(process->collector), REWARDCOLLECTOR_LATEST);
}

void SPSALearning_reset(SPSALearning_t* process, int random) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		if(random) process->theta[i] = ((float)rand())/(((float)RAND_MAX)+0.1f); //initialize a random solution
		else process->theta[i] = 0.5f;
	}
	process->state=0;
	randomizeDelta(process);
}

void SPSALearning_update(SPSALearning_t* process) {
	if(!SPSALearning_isReady(process)) {
		#ifdef PRINTF
		ase_printf("Warning: #1 in SPSALearning \n"); //  learning process not updated since no reward value received
		#endif
		return;
	}
	float reward = rewardCollector_getReward(&process->collector);
	rewardCollector_resetReward(&process->collector);

	if(process->state==0) {
		process->yPlus = reward;
		process->state++;
	}
	else if(process->state==1) {
		process->yMinus = reward;
		updateTheta(process);
		randomizeDelta(process);
		process->state = 0;
	}
}

void SPSALearning_ignoreUpdate(SPSALearning_t* process) {
	rewardCollector_resetReward(&process->collector);
}

float SPSALearning_getThetaNonPerturbed(SPSALearning_t* process, int index) {
	return process->theta[index];
}

float SPSALearning_getTheta(SPSALearning_t* process, int index, int wrap) {
	if(process->state==0) {
		float theta = process->theta[index]+process->ck*process->delta[index];
		if(wrap) return wrapClamp(theta);
		else return clamp(theta);
	}
	else { //state==1
		float theta = process->theta[index]-process->ck*process->delta[index];
		if(wrap) return wrapClamp(theta);
		else return clamp(theta);
	}
}

void SPSALearning_setTheta(SPSALearning_t* process, float* theta) {
	int i;
	for(i=0;i<process->nParameters;i++) {
		float s = clamp(theta[i]);
		if(s!=theta[i]) {
			#ifdef PRINTF
			ase_printf("Warning: #2 in SPSALearning \n"); //  Only use parameters between 0-1 (scale later)
			#endif
		}
		process->theta[i] = s;
	}
}

float SPSALearning_getReward(SPSALearning_t* process) {
	return rewardCollector_getReward(&process->collector);
}

void SPSALearning_collectReward(SPSALearning_t* process, float reward) {
	rewardCollector_collect(&process->collector, reward);
}

int SPSALearning_isReady(SPSALearning_t* process) {
	return rewardCollector_isUpdated(&process->collector);
}
