#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/control/strategies/ffANN/ffANN.h>

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
	 //input[index][0] = 0; //time since event
	 //input[index][1] = 0; //top distance
	 //input[index][2] = 0; //left distance
	 //input[index][3] = 0; //right distance
	 input[index][4] = 0; //mic
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

float myError(signed char* x, signed char* y, int length) {
	float dist = 0;
	for(int i=0;i<length;i++) {
		float diff = abs(x[i]-y[i]);
		if(diff>10) dist++;
	}
	return dist/length;
}
float myErrorFloat(signed char* x, float* y, int length) {
	float dist = 0;
	for(int i=0;i<length;i++) {
		float diff = abs(x[i]-y[i]);
		if(diff>10) dist++;
	}
	return dist/length;
}

bool compactPrint = true;
static char* baseDir = "trainingdata/car-escape/exp";

float knnCompare(int trainIndex, int testIndex, int k) {
	char dir[80];
	sprintf (dir, "%s%i/data_sets.log", baseDir, trainIndex);
	int nSets = getData(dir);

	if(!compactPrint) ase_printf("Loaded n =%i Training sets\n", nSets);
	for(int i=0;i<nSets;i++) {
	  //printf("(%i %i %i %i %i) ->(%i %i %i %i)\n",input[i][0],input[i][1],input[i][2],input[i][3],input[i][4],output[i][0],output[i][1],output[i][2],output[i][3]);
	  kNN_addTraningSet(kNN_behavior, input[i], 5, output[i], 4);
	}
	kNN_removeContradictions(kNN_behavior, 5, 4);
	int loadedSets = nSets;
	int storedSets = kNN_behavior->nSets;
	float reduction = ((float)storedSets)/((float)loadedSets);
	sprintf (dir, "%s%i/data_sets.log", baseDir, testIndex);

	nSets = getData(dir);
	if(!compactPrint) ase_printf("Loaded n =%i Testing sets\n", nSets);

	signed char knnout[4];
	if(!compactPrint) ase_printf("\n K=: %i \n",k);
	kNN_setK(kNN_behavior, k);
	float errorSum = 0;
	int success = 0;
	for(int i=0;i<nSets;i++) {
		kNN_getOutput(kNN_behavior, input[i], 5, knnout, 4);
		errorSum+=myError(output[i],knnout,4);

		/*error = cheDist(output[i],knnout,4);
		errorSum+=error;
		if(error<=25) success++;
		else {
				//ase_printf("Error:\n");
				//ase_printf("    %i %i %i %i\n",output[i][0],output[i][1],output[i][2],output[i][3]);
				//ase_printf("    %i %i %i %i\n\n",knnout[0],knnout[1],knnout[2],knnout[3]);
		}*/
	}

	if(!compactPrint) ase_printf("\nAverage error: %f \n",errorSum/nSets);
	if(!compactPrint) ase_printf("Success percent: %f \n",100.0f*((float)success)/nSets);
	//if(compactPrint) ase_printf("{%i, %i, %i, %f, %f},\n", trainIndex, testIndex, k, ((float)success)/nSets,1-errorSum/nSets);
	if(compactPrint) ase_printf("{%i, %i, %i, %f, %i, %i, %f},\n", trainIndex, testIndex, k, 1-errorSum/nSets, loadedSets, storedSets, reduction);
	kNN_clearMemory(kNN_behavior);
	//return ((float)success)/nSets;
	return 1-errorSum/nSets;
}

void knnTest(int nExp, int kMax) {
	for(int k=1;k<=kMax;k++) {
		ase_printf("******** k = %i ********* \n",k);
		int nTotal=0;
		float result = 0;
		for(int i=1;i<=nExp;i++) {
			for(int j=1;j<=nExp;j++) {
				if(i!=j)
				{
					result+=knnCompare(i,j,k);
					nTotal++;
				}
			}
		}
		ase_printf("AVERAGE = %f percent\n\n",100*result/nTotal);
	}
}

void annTest(int trainIndex, int testIndex) {
	float anninput[5];
	float annoutput[4];
	float errorSum = 0;
	ANN_t *ann = ANN_New(5, 4, 6, 0, 0, &ANN_Sigmoid, 10.0, 0.0);
	ANN_RandomizeWeights(ann);
	ANN_DeleteRecurrentConnections(ann);

	char dir[80]; sprintf (dir, "%s%i/data_sets.log", baseDir, trainIndex);
	int nTrainSets = getData(dir);
	for(int n=0;n<5000;n++) {
		float errorSum=0;
		for(int i=0;i<nTrainSets;i++) {
			for(int j=0;j<5;j++) anninput[j] = input[i][j];
			for(int j=0;j<4;j++) annoutput[j] = output[i][j];
			errorSum += ANN_TrainBackProp(ann, anninput, annoutput);
		}
		if(n%1000==0) ase_printf("%i: %f error\n", n, errorSum/nTrainSets);
	}


	sprintf (dir, "%s%i/data_sets.log", baseDir, testIndex);
	int nTestSets = getData(dir);
	for(int i=0;i<nTestSets;i++) {
		for(int j=0;j<5;j++) anninput[j] = input[i][j];
		ANN_Execute(ann, anninput, annoutput);
		for(int j=0;j<4;j++) annoutput[j] *= 100; //scale up
		float error =myErrorFloat(output[i], annoutput,4);
		errorSum+=error;
		//ase_printf("TEST: (%i, %i, %i, %i, %i) -> (%i, %i, %i, %i)\n", input[i][0],input[i][1],input[i][2],input[i][3],input[i][4],output[i][0],output[i][1],output[i][2],output[i][3]);
		//ase_printf("ANN : (%i, %i, %i, %i, %i) -> (%i, %i, %i, %i)\n", (int)anninput[0],(int)anninput[1],(int)anninput[2],(int)anninput[3],(int)anninput[4],(int)annoutput[0],(int)annoutput[1],(int)annoutput[2],(int)annoutput[3]);
		//ase_printf("ERR : %f\n\n", error);
	}
	ase_printf("{%i, %i, %f},\n", trainIndex, testIndex, 1-errorSum/nTestSets);

	//ANN_Print(ann);
}

float ffAnnCompare(int trainIndex, int testIndex) {
	float anninput[5];
	float annoutput[4];
	float errorSum = 0;
	//ffANN_t *ann = ffANN_New(5, 4, 6, 1.00f, 1.00f);
	ffANN_t *ann = ffANN_New(5, 4, 8, 1.0f, 1.0f);


	char dir[80]; sprintf (dir, "%s%i/data_sets.log", baseDir, trainIndex);
	int nTrainSets = getData(dir);
	for(int n=0;n<1000;n++) {
		float errorSum=0;
		for(int i=0;i<nTrainSets;i++) {
			int tIndex = rand()%nTrainSets;
			for(int j=0;j<5;j++) anninput[j] = ((float)input[tIndex][j])/100;
			for(int j=0;j<4;j++) annoutput[j] = ((float)output[tIndex][j]+100)/200;
			errorSum += ffANN_Train(ann, anninput, annoutput);
		}
		//if(n%1000==0) ase_printf("%i: %f error\n", n, errorSum/nTrainSets);
	}


	sprintf (dir, "%s%i/data_sets.log", baseDir, testIndex);
	int nTestSets = getData(dir);
	for(int i=0;i<nTestSets;i++) {
		for(int j=0;j<5;j++) anninput[j] = ((float)input[i][j])/100;
		ffANN_Execute(ann, anninput, annoutput);
		for(int j=0;j<4;j++) annoutput[j] = 200*annoutput[j]-100; //scale up
		float error =myErrorFloat(output[i], annoutput, 4);
		errorSum+=error;
		//ase_printf("TEST: (%i, %i, %i, %i, %i) -> (%i, %i, %i, %i)\n", input[i][0],input[i][1],input[i][2],input[i][3],input[i][4],output[i][0],output[i][1],output[i][2],output[i][3]);
		//ase_printf("ANN : (%i, %i, %i, %i, %i) -> (%i, %i, %i, %i)\n", (int)anninput[0],(int)anninput[1],(int)anninput[2],(int)anninput[3],(int)anninput[4],(int)annoutput[0],(int)annoutput[1],(int)annoutput[2],(int)annoutput[3]);
		//ase_printf("ERR : %f\n\n", error);
	}
	ase_printf("{%i, %i, %f},\n", trainIndex, testIndex, 1-errorSum/nTestSets);
	return 1-errorSum/nTestSets;
}

void ffAnnTest(int nExp) {
	int nTotal=0;
	float result = 0;
	for(int i=1;i<=nExp;i++) {
		for(int j=1;j<=nExp;j++) {
			if(i!=j)
			{
				result+=ffAnnCompare(i,j);
				nTotal++;
			}
		}
	}
	ase_printf("AVERAGE = %f percent\n\n",100*result/nTotal);
}

void controller_init() {
	ase_printf("starting...\n");
	srand(time(NULL));
	kNN_behavior = malloc(sizeof(kNN_t));
	playback_data = malloc(sizeof(Playback_t));
	kNN_init(kNN_behavior, 1);
	Playback_init(playback_data);
	int nExp = 1;
	//baseDir = "trainingdata/car-escape/exp";nExp=4;

	kNN_setMode(kNN_behavior, KNN_MODE_MEAN);
	//kNN_setMode(kNN_behavior, KNN_MODE_MOST_FREQUENT_SET);
	//kNN_setMode(kNN_behavior, KNN_MODE_MOST_FREQUENT_OUT);

	ase_printf("kNN Escape: \n");
	baseDir = "trainingdata/testRun/escape/";nExp=5;
	knnTest(nExp,1);
	ase_printf("\n\n");

	ase_printf("kNN Follow: \n");
	baseDir = "trainingdata/testRun/follow/";nExp=5;
	knnTest(nExp,1);
	ase_printf("\n\n");

	ase_printf("ANN Escape: \n");
	baseDir = "trainingdata/testRun/escape/";nExp=5;
	ffAnnTest(nExp);
	ase_printf("\n\n");

	ase_printf("ANN Follow: \n");
	baseDir = "trainingdata/testRun/follow/";nExp=5;
	ffAnnTest(nExp);
	ase_printf("\n\n");

	ase_printf("exit...\n");
	exit(0);
}
