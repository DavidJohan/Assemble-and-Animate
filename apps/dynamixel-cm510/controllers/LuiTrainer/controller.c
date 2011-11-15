#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/strategies/ANN/ann.h>

#include <ase/control/controller.h>

static kNN_t* kNN_behavior;
static Playback_t* playback_data;

static signed char input[500][5];
static signed char output[500][4];

int getData(char* dir) {
	char line[80];
	FILE *fr;
	fr = fopen (dir, "rt");
	int t,index = 0;
	while(fgets(line, 80, fr) != NULL) {
	 sscanf (line,"%i, %i, %i, %i, %i, %i, %i, %i, %i, %i",&t,(int*)(&input[index][0]),(int*)(&input[index][1]),(int*)(&input[index][2]),(int*)(&input[index][3]),(int*)(&input[index][4]),(int*)(&output[index][0]),(int*)(&output[index][1]),(int*)(&output[index][2]),(int*)(&output[index][3]));
	 index++;
	}
	fclose(fr);  /* close the file prior to exiting the routine */
	return index;

}
float eucDist(signed char* x, signed char* y, int length) {
	float sum = 0;
	for(int i=0;i<length;i++) {
		sum+=(x[i]-y[i])*(x[i]-y[i]);
	}
	return sqrt(sum);
}
//Chebyshev distance
float cheDist(signed char* x, signed char* y, int length) {
	float max = 0;
	for(int i=0;i<length;i++) {
		float diff = abs(x[i]-y[i]);
		if(diff>max) max = diff;
	}
	return max;
}


bool compactPrint = true;
void knnCompare(int trainIndex, int testIndex, int k) {
	char dir[80]; sprintf (dir, "trainingdata/car-escape/exp%i/data_sets.log", trainIndex);
	int nSets = getData(dir);
	if(!compactPrint) ase_printf("Loaded n =%i Training sets\n", nSets);
	for(int i=0;i<nSets;i++) {
	  //printf("(%i %i %i %i %i) ->(%i %i %i %i)\n",input[i][0],input[i][1],input[i][2],input[i][3],input[i][4],output[i][0],output[i][1],output[i][2],output[i][3]);
	  kNN_addTraningSet(kNN_behavior, input[i], 5, output[i], 4);
	}

	sprintf (dir, "trainingdata/car-escape/exp%i/data_sets.log", testIndex);
	nSets = getData(dir);
	if(!compactPrint) ase_printf("Loaded n =%i Testing sets\n", nSets);

	signed char knnout[4];
	if(!compactPrint) ase_printf("\n K=: %i \n",k);
	kNN_setK(kNN_behavior, k);
	float error,errorSum = 0;
	int success = 0;
	for(int i=0;i<nSets;i++) {
		kNN_getOutput(kNN_behavior, input[i], 5, knnout, 4);
		error = cheDist(output[i],knnout,4);
		errorSum+=error;
		/*ase_printf("%i ,",(int)error);
		if(i%15==0) ase_printf("\n");*/
		if(error<=25) success++;
	}
	if(!compactPrint) ase_printf("\nAverage error: %f \n",errorSum/nSets);
	if(!compactPrint) ase_printf("Success percent: %f \n",100.0f*((float)success)/nSets);
	if(compactPrint) ase_printf("{%i, %i, %i, %f},\n", trainIndex, testIndex, k, ((float)success)/nSets);
}

void knnTest() {
	int nExp = 4;
	for(int i=1;i<=nExp;i++) {
		for(int j=1;j<=nExp;j++) {
			if(i!=j) knnCompare(i,j,1);
		}
	}
}
void annTest() {
	float input[5];
	float output;
	ANN_t *ann = ANN_New(5, 4, 6, 0, 0, &ANN_Sigmoid, 10.0, 0.0);
	ANN_RandomizeWeights(ann);
	ANN_DeleteRecurrentConnections(ann);
	input[0] = 0.1;
	input[1] = 0.312;
	ANN_Execute(ann, input, &output);
	ANN_Print(ann);
	int i;
	for(i=0;i<10;i++) {
		ase_printf("Output: %3.2f\n", output);
		ANN_Execute(ann, input, &output);
	}
}


void controller_init() {
	ase_printf("starting...\n");
	kNN_behavior = malloc(sizeof(kNN_t));
	playback_data = malloc(sizeof(Playback_t));
	kNN_init(kNN_behavior, 1);
	Playback_init(playback_data);
	knnTest();
	//annTest();
	ase_printf("exit...\n");
	exit(0);
}
