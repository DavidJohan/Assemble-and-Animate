#define	  ASE_CTRL
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ffANN/ffANN.h>

#define ONE		0.9f
float tIn[4][2] = {{0,0},{0,ONE},{ONE,0},{ONE,ONE}};
float tOut[4][1] = {{0},{ONE},{ONE},{0}}; //xor
int nTrainSets = 4;

void printXORTask(ffANN_t* ann) {
	ase_printf("\n****** XOR *******\n");
	float out[1];
	for(int i=0;i<nTrainSets;i++) {
		ffANN_Execute(ann, tIn[i], out);
		ase_printf("%i: (%f, %f)->(%f)\n", i, tIn[i][0], tIn[i][1],out[0]);
	}
	ase_printf("******************\n");
}

void controller_init() {
	srand(time(NULL));
	//ffANN_t *ann = ffANN_New(2, 1, 2, 0.15f, 0.2f);
	ffANN_t *ann = ffANN_New(2, 1, 4, 1.0f, 1.0f);
	printXORTask(ann);
	ffANN_print(ann);
	ffANN_setLearningType(ann, ffANN_LEARNING_TYPE_SPSA);
	ffANN_print(ann);
	for(int n=0;n<10000;n++) {
		float errorSum = ffANN_TrainFullSet(ann, nTrainSets, tIn, tOut);
		if(n%1000==0) ase_printf("%i: %f error\n", n, errorSum/nTrainSets);
	}
	ffANN_print(ann);
	printXORTask(ann);
	exit(0);
}
