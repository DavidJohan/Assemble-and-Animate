/**
 * \file kNN.h
 * This file provides an implementation of k-nearest neighbors
 *
 * \date December 2010
 * \author David Johan Christensen, Center for Playware, Technical University of Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef KNN_H_
#include <ase/config/ConfigASE.h>
#define MAX_KNN_SETS		100
#define MAX_KNN_INPUTS		5
#define MAX_KNN_OUTPUTS		4

#define KNN_MODE_MEAN 					0
#define KNN_MODE_MOST_FREQUENT_SET		1
#define KNN_MODE_MOST_FREQUENT_OUT		2

typedef struct {
	signed char input[MAX_KNN_INPUTS]; //better data structure nessecary
	signed char output[MAX_KNN_OUTPUTS];
	int dist; //long dist
	//long timeMs;
	bool isTaken;
} kNNTSet_t;


typedef struct {
	short label;
	kNNTSet_t sets[MAX_KNN_SETS]; //better data structure nessecary
	int nSets;
	int k;
	int mode;
} kNN_t;

/**
 * Initialize the learning process with default parameters
 */
void kNN_init(kNN_t* process, int k);

void kNN_clearMemory(kNN_t* process);
/**
 * Computes k nearest neighbor and returns the result in the output?
 * The size of input/output may change over time - how to handle?
 */
void kNN_getOutput(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs);

/**
 * Adds a new traning set to the kNN process
 * How to handle too many traning sets?
 * How to handle distribution of data sets?
 */
bool kNN_addTraningSet(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs);

/**
 * Return the amount of information in the training set to evaluate if the set should
 * be saved.
 */
char kNN_trainingSetInformation(kNN_t* process, signed char* input, int nInputs, signed char* output, int nOutputs);


/**
 * Set k - number of neighbors to consider
 */
bool kNN_setK(kNN_t* process, unsigned char k);
/**
 * Set the mode of the algorithm:
 * 0 = average output of k neighbors
 */
bool kNN_setMode(kNN_t* process, unsigned char mode);

#define KNN_H_

#endif
