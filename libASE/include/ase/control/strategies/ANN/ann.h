/*
 * =====================================================================================
 *
 *       Filename:  ann.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-05-05 15:08:38
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mikael Moghadam (mimog), binarybusiness [at] gmail [dot] com
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef ANN_H
#define ANN_H

#include <stdbool.h>

#define DOUBLE_MIN (-(FLT_MAX / 2))
#define DOUBLE_MAX (FLT_MAX / 2)

typedef struct ANN_s {
        unsigned int nr_inputs;
        unsigned int nr_hidden;
        unsigned int nr_outputs;
        unsigned int nr_max_con_fw;
        unsigned int nr_max_con_rc;
        float (*ActivationFunction)(float x);
        float w_max_init;
        float w_min_init;
        float **weights;
        float *neuron_outputs;         /* Contiains neuron outputs. We look in this array to get outputs of other neurons when calculating inputs to a neuron.
                                        * This way we get recurrent connections automatically (if a neuron is ahead in the network, we have its old value in this array!) */
} ANN_t;

void ANN_Print(ANN_t *);
bool ANN_MinimizeNrForwardWeights(ANN_t *nn);
void ANN_MinimizeNrRecurrentWeights(ANN_t *nn);
void ANN_ResetNeuronState(ANN_t *n);
float ANN_Sigmoid(float x);
float ANN_Tanh(float x);
void ANN_Execute(ANN_t *nn, float *inputs, float *output);
void ANN_RandomizeWeights(ANN_t *nn);
ANN_t *ANN_New(unsigned int nr_inputs,
                unsigned int nr_outputs,
                unsigned int nr_hidden,
                unsigned int nr_max_con_forward,
                unsigned int nr_max_con_recurrent,
                float (*ActivationFunction)(float x),
                float w_max,
                float w_min);
float ANN_FindLargestForwardWeightSmallerThanX(ANN_t *nn, float X);
float ANN_FindLargestRecurrentWeightSmallerThanX(ANN_t *nn, float X);
bool ANN_AllInputsLeadToOutput(ANN_t *nn);
void ANN_ScaleHiddenNeuronWeights(ANN_t *nn, float factor);
void ANN_ScaleAllNeuronWeights(ANN_t *nn, float factor);
void ANN_RandomizeRecurrentWeights(ANN_t *nn);
void ANN_RandomizeForwardWeights(ANN_t *nn);
void ANN_MinimizeNrRecurrentWeights(ANN_t *nn);
void ANN_DeleteRecurrentConnections(ANN_t *nn);
bool ANN_MinimizeNrForwardWeights(ANN_t *nn);
void ANN_MinimizeNrWeights(ANN_t *nn);
ANN_t *ANN_Copy(ANN_t *nn);
void ANN_Delete(void **n);
void ANN_Print(ANN_t *nn);
void ANN_ClearConnections(ANN_t *nn);
void ANN_Connect(ANN_t *nn, unsigned int n1, unsigned int n2, float w);
void ANN_Mutate(ANN_t *nn, float probability);
ANN_t *ANN_Crossover(ANN_t *nn1, ANN_t *nn2);

#endif
