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
	process->mode =KNN_MODE_MEAN;
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

static bool equals(signed char* s1, signed char* s2, int nElements) {
	return false;
}


void kNN_getOutput(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs) {
	if(nInputs==0) {
	  	int i;
		for(i=0;i<nOutputs;i++) {
			output[i]= 0;
		}
		return;
	}
	
	for(int i=0;i<MAX_KNN_SETS;i++) {
		if(process->sets[i].isTaken) {
			process->sets[i].dist = dist(input, process->sets[i].input, nInputs);
		}
		else {
			process->sets[i].dist = 10001;
		}
	}

	qsort(process->sets, process->nSets, sizeof(kNNTSet_t), (int(*)(const void*, const void*))myCmp);
	int K = (process->k<=process->nSets)?process->k:process->nSets;

	if(process->mode==KNN_MODE_MEAN) {
		//selects the mean output amongst the k sets
		long lOutput[MAX_KNN_OUTPUTS];
		for(int i=0;i<nOutputs;i++) {
			lOutput[i]= 0;
			for(int k=0;k<K;k++) {
				lOutput[i] += process->sets[k].output[i];
			}
		}
		for(int i=0;i<nOutputs;i++) {
		  	output[i]= lOutput[i]/K;
		}
	}
	else if(process->mode==KNN_MODE_MOST_FREQUENT_SET) {
		//selects the most frequent category amongst the k sets (considering the whole output)
		char freq[K];
		for(int i=0;i<K;i++) {
			freq[i]=0;
		}
		int maxVotesIndex=0, maxVotes=0;
		for(int i=0;i<K;i++) {
			int votes = 0;
			for(int j=0;j<K;j++) {
				if(i!=j) {
					if(equals(process->sets[i].output, process->sets[j].output, nOutputs)) {
						votes++;
					}
				}
			}
			if(votes>maxVotes) {
				maxVotes = votes;
				maxVotesIndex = i;
			}
		}
		if(maxVotes == 0) { //all sets are different (should be a redundant check)
			maxVotesIndex = 0; //select nearest
		}
		for(int i=0;i<nOutputs;i++) {
			output[i] = process->sets[maxVotesIndex].output[i];
		}
	}
	else if(process->mode==KNN_MODE_MOST_FREQUENT_OUT) {
		//selects the most frequent category for each individual output amongst the k sets
		unsigned char freq[255];
		for(int i=0;i<nOutputs;i++) {
			for(int i=0;i<256;i++) {
				freq[i]=0;
			}
			for(int k=0;k<K;k++) {
				int index = process->sets[k].output[i] + 128;
				freq[index]++;
			}
			unsigned char maxIndex = 0, maxFreq = 0;
			for(int i=0;i<256;i++) {
				if(freq[i]>maxFreq) {
					maxIndex = i;
					maxFreq = freq[i];
				}
			}
			output[i] = maxIndex - 128;
		}
	}
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
	/*ase_printf("kNN (nSets = %i): (",process->nSets);
	for(i=0;i<nInputs;i++) {
	  ase_printf("%i ", input[i]);
	}
	ase_printf(") -> (");
	for(i=0;i<nOutputs;i++) {
	  ase_printf("%i ", output[i]);
	}
	ase_printf(")\n");*/
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
