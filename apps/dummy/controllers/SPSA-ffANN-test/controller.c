#define	  ASE_CTRL
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ffANN/ffANN.h>

float tIn[4][3] = {{0,0},{0,1},{1,0},{1,1}};
float tOut[4][1] = {{0},{1},{1},{0}}; //xor

void printXORTask(ffANN_t* ann) {
	ase_printf("\n****** XOR *******\n");
	float out[1];
	for(int i=0;i<4;i++) {
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

	for(int n=0;n<10000;n++) {
		float errorSum=0;
		for(int i=0;i<4;i++) {
			errorSum += ffANN_Train(ann, tIn[i], tOut[i]);
		}
		if(n%1000==0) ase_printf("%i: %f error\n", n, errorSum/4);
	}
	printXORTask(ann);
	exit(0);
}
