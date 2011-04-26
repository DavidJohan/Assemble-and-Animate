#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/Message.h>
#include <ase/kNN.h>

#define N_TRAIN		25
static kNN_t kNNprocess;
char tInputs[N_TRAIN][2];
char tOutputs[N_TRAIN][1];

void kNN_initTrainingData() {
	srand (getRandomSeed());
	int i;
	for(i=0;i<N_TRAIN;i++) {
		tInputs[i][0] = rand()%100;
		tInputs[i][1] = rand()%100;
		tOutputs[i][0] = (tInputs[i][0] + tInputs[i][1])/2;
	}
}

void kNN_train() {
	int i;
	for(i=0;i<N_TRAIN/2+1;i++) {
			kNN_addTraningSet(&kNNprocess, tInputs[i], 2, tOutputs[i], 1);
	}
}

void kNN_trainWithInfo() {
	srand (getRandomSeed());
	int i, nTrials=0;
	for(i=0;i<N_TRAIN/2+1;) {
		tInputs[i][0] = rand()%100;
		tInputs[i][1] = rand()%100;
		tOutputs[i][0] = (tInputs[i][0] + tInputs[i][1])/2;
		char information = kNN_trainingSetInformation(&kNNprocess, tInputs[i], 2, tOutputs[i], 1);
		if(information>10) {
			kNN_addTraningSet(&kNNprocess, tInputs[i], 2, tOutputs[i], 1);
			//printf("%i, ", information);
			i++;
		}
		nTrials++;
	}
	//printf("\n");
	//printf("nTrials = %i\n",nTrials);
	for(;i<N_TRAIN;i++) {
		tInputs[i][0] = rand()%100;
		tInputs[i][1] = rand()%100;
		tOutputs[i][0] = (tInputs[i][0] + tInputs[i][1])/2;
	}
}
//(n=100,k=1)=6.38775, (n=1000,k=1) = 7.08
void kNN_act(int k)  {
	int i;
	char output[1];
	float errorSum = 0;
	int nSets=0;
	for(i=N_TRAIN/2+1;i<N_TRAIN;i++) {
		kNN_getOutput(&kNNprocess, tInputs[i], 2,  output, 1);
		//printf("%i: %i vs %i \n", i, (int)output[0], (int)tOutputs[i][0]);
		errorSum+=abs(output[0]-tOutputs[i][0]);
		nSets++;
	}
	printf("%i: Average error = %f\n",k,errorSum/nSets);
	/*signed char result[3];
	short input[3], output[3];
	input[0]=result[0];input[1]=result[1];input[2]=result[2];
	kNN_getOutput(&kNNprocess, input, output);*/
}

void controller_init() {
	int k;
	for(k=1;k<15;k++) {
		kNN_init(&kNNprocess, k);
		if(0) {
			kNN_initTrainingData();
			kNN_train();
		}
		else {
			kNN_trainWithInfo();
		}
		kNN_act(k);
	}
}
