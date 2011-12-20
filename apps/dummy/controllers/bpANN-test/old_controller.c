#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ANN/ann.h>

float tIn[4][3] = {{0,0},{0,1},{1,0},{1,1}};
float tOut[4][1] = {{0},{1},{1},{0}}; //xor

void printXORTask(ANN_t* ann) {
	ase_printf("\n****** XOR *******\n");
	float out[1];
	for(int i=0;i<4;i++) {
		ANN_Execute(ann, tIn[i], out);
		ase_printf("%i: (%f, %f)->(%f)\n", i, tIn[i][0], tIn[i][1],out[0]);
	}
	ase_printf("******************\n");
}
void controller_init() {
	float errorSum = 0;
	ANN_t *ann = ANN_New(3, 1, 2, 0, 0, &ANN_Sigmoid, 1.0, 0.0);
	ANN_RandomizeWeights(ann);
	//ANN_DeleteRecurrentConnections(ann);
	ANN_MakeFeedForward(ann);
	/*ann->weights[0][2] = 1;
	ann->weights[0][3] = -1;
	ann->weights[1][2] = -1;
	ann->weights[1][3] = 1;
	ann->weights[2][4] = 1;
	ann->weights[3][4] = 1;*/

	printXORTask(ann);
	ANN_Print(ann);
	for(int n=0;n<10000;n++) {
		float errorSum=0;
		for(int i=0;i<4;i++) {
			errorSum += ANN_TrainBackProp(ann, tIn[i], tOut[i]);
		}
		if(n%1000==0) ase_printf("%i: %f error\n", n, errorSum/4);
	}
	printXORTask(ann);
	ANN_Print(ann);
	exit(0);
}
