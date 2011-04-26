#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/tools/RewardCollector/RewardCollector.h>

void kNN_init(kNN_t* process, int k) {
  	process->nSets=0;
	process->k = k;
	process->mode =0;
	int i;
	for(i=0;i<MAX_KNN_SETS;i++) {
		process->sets[i].isTaken= false;
	}
	ase_printf("kNN uses %i bytes of ram!\n", sizeof(kNN_t));
}

void kNN_clearMemory(kNN_t* process) {
	int i;
	for(i=0;i<MAX_KNN_SETS;i++) {
		process->sets[i].isTaken= false;
	}
	process->nSets = 0;
	ase_printf("kNN uses %i bytes of ram!\n", sizeof(kNN_t));
}

bool kNN_setK(kNN_t* process, unsigned char k) {
  	if(k>0) {
    		process->k = k;
		return true;
	}
	return false;
}
bool kNN_setMode(kNN_t* process, unsigned char mode) {
	process->mode = mode;
	return true;
}

static long dist(signed char* a, signed char* b, int length) {
  	long distSum=0;
	int i;
  	for(i=0;i<length;i++) {
	  distSum += (a[i]-b[i])*(a[i]-b[i]);
	}
	return distSum; //no need to compute the square root: sqrt((float)distSum)
}
static int myCmp(kNNTSet_t* set1, kNNTSet_t* set2 ) {
	if(set1->dist==set2->dist) return 0;
	else if(set1->dist>set2->dist) return 1;
	else return -1;
}

void kNN_getOutput(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs) {
	if(nInputs==0) {
	  	int i;
		for(i=0;i<nOutputs;i++) {
			output[i]= 0;
		}
		return;
	}
	
	long lOutput[MAX_KNN_SETS]; //stor fejl her!
	int i,k;
	for(i=0;i<MAX_KNN_SETS;i++) {
		if(process->sets[i].isTaken) {
			process->sets[i].dist = dist(input, process->sets[i].input, nInputs);
		}
		else {
			process->sets[i].dist = 10001;
		}
	}
	/*ase_printf("\n********BEFORE********\n");
	for(i=0;i<10;i++) {
		ase_printf("%li ,",process->sets[i].dist);
		if(i%5==4) ase_printf("\n");
	}*/
	qsort(process->sets, process->nSets, sizeof(kNNTSet_t), (int(*)(const void*, const void*))myCmp);

	/*ase_printf("\n********AFTER********\n");
	for(i=0;i<10;i++) {
		ase_printf("%li ,",process->sets[i].dist);
		if(i%5==4) ase_printf("\n");
	}*/
	//printf("Sample (%i,%i)->%i\n",input[0],input[1],output[0]);
	//printf("kNN    (%i,%i)->%i\n",process->sets[0].input[0],process->sets[0].input[1],process->sets[0].output[0]);
	//printf("Dist = %f\n", process->sets[0].dist);
	int n = (process->k<=process->nSets)?process->k:process->nSets;

	//long dist = process->sets[n].dist; 
	//while(dist>=process->sets[n].dist && n<=process->nSets) n++;
	//ase_printf("kNN: adaptive k = %i\n", n);
	
	for(i=0;i<nOutputs;i++) {
		lOutput[i]= 0;
		for(k=0;k<n;k++) {
			lOutput[i] += process->sets[k].output[i];
		}
	}

	if(process->mode==0) {//possibilities: average, median, map to categories, ...
		for(i=0;i<nOutputs;i++) {
			lOutput[i]= 0;
			for(k=0;k<n;k++) {
				lOutput[i] += process->sets[k].output[i];
			}
		}
		for(i=0;i<nOutputs;i++) {
		  	output[i]= lOutput[i]/n;
		}
	}
	ase_printf("kNN output: (");
	for(i=0;i<nInputs;i++) {
	  ase_printf("%i ", input[i]);
	}
	ase_printf(") -> (");
	for(i=0;i<nOutputs;i++) {
	  ase_printf("%i ", output[i]);
	}
	ase_printf(")\n");
	
	//ase_printf("kNN output: %i -> (%i, %i)\n", input[0], output[0], output[1]);
}

int findFreeIndex(kNN_t* process) {
	int i;
	for(i=0;i<MAX_KNN_SETS;i++) {
		if(process->sets[i].isTaken==false) return i;
	}
	return -1;
}

bool kNN_addTraningSet(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs) {
  	int index = findFreeIndex(process);
	if(index==-1) { 
		return false; //free oldest or least information?
	}	
	else {
		process->sets[index].isTaken = true;
		process->nSets++;
	}
	int i;
	for(i=0;i<MAX_KNN_INPUTS;i++) {
		if(i<nInputs) {
			process->sets[index].input[i] = input[i];
		}
		else {
			process->sets[index].input[i] = 0;
		}
	}
	for(i=0;i<MAX_KNN_OUTPUTS;i++) {
		if(i<nOutputs) {
			process->sets[index].output[i] = output[i];
		}
		else {
			process->sets[index].output[i] = 0;
		}
	}
	ase_printf("kNN (nSets = %i): (",process->nSets);
	for(i=0;i<nInputs;i++) {
	  ase_printf("%i ", input[i]);
	}
	ase_printf(") -> (");
	for(i=0;i<nOutputs;i++) {
	  ase_printf("%i ", output[i]);
	}
	ase_printf(")\n");
	return true;
  	
}

static long information(signed char* a, signed char* b, int length) {
  	long maxDiff=0;
	int i;
  	for(i=0;i<length;i++) {
		long diff = abs(a[i]-b[i]); //what about non impordant sensor input like compass?
		if(maxDiff<diff) {
			maxDiff=diff;
		}
	}
	return maxDiff;
}

char kNN_trainingSetInformation(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs) {
	if(process->nSets==0) return 100;
	int i;
	for(i=0;i<MAX_KNN_SETS;i++) {
		if(process->sets[i].isTaken) {
			process->sets[i].dist = information(input, process->sets[i].input, nInputs);
			process->sets[i].dist += information(output, process->sets[i].output, nOutputs);
		}
		else {
			process->sets[i].dist = 1000;
		}
	}
	qsort(process->sets, process->nSets, sizeof(kNNTSet_t), (int(*)(const void*, const void*))myCmp);
	return process->sets[0].dist;
}
