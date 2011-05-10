/*
 * =====================================================================================
 *
 *       Filename:  ann.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-05-05 15:40:23
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mikael Moghadam (mimog), binarybusiness [at] gmail [dot] com
 *        Company:  
 *         Licese:  GPLv3
 *
 * =====================================================================================
 */

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/MemManager/MemManager.h>

#define EXIT_FAILURE 1

/*-----------------------------------------------------------------------------
 * NEURAL NETWORK
 *-----------------------------------------------------------------------------*/
void ANN_ResetNeuronState(ANN_t *n)
{
        int n_total = n->nr_inputs + n->nr_hidden + n->nr_outputs;
        for (int i = 0; i < n_total; i++) {
                n->neuron_outputs[i] = 0;
        }
}

float ANN_Sigmoid(float x)
{
        static int K = 1;
        return (1.0 / (1 + exp(-K * x)));
}

float ANN_Tanh(float x)
{
        return (2 / (1 + exp(-2 * x)) - 1);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ANN_Execute
 *  Description:
 *   Parameters:
 *      float ouput  - The output of the network is copied here and it must therefore
 *                      be an array with the same size as nr_outputs.
 *
 *      float inputs - The inputs to the network. The size must match the number of
 *                      inputs in the network.
 * =====================================================================================
 */
void ANN_Execute(ANN_t *nn, float *inputs, float *output)
{

        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        float temp_neuron_outputs[neurons_total];
        float sum = 0;
        for (int i = 0; i < neurons_total; i++) {
                if (i < nn->nr_inputs) { /* Its an input neuron */
                        sum = inputs[i]; //nn->neuron_outputs[i] = inputs[i];
                } else { /* Its a hidden neuron or an output neuron */
                        sum = 0;
                }

                for (int j = 0; j < neurons_total; j++) {
                        if (nn->weights[j][i] > DOUBLE_MIN && nn->weights[j][i] < DOUBLE_MAX) {
                                sum += nn->weights[j][i] * nn->neuron_outputs[j];
                        }
                }

                /* Now that we have summed all inputs to the neuron, we can get its output */
                //nn->neuron_outputs[i] = nn->ActivationFunction(sum); //ANN_sigmoid(sum);
                temp_neuron_outputs[i] = nn->ActivationFunction(sum);
                if (nn->neuron_outputs[i] > 2) {
                        ase_printf("OUTPUT NEURON HAS VALUE LARGER THAN 1: %3.2f  SUM: %3.2f\n", nn->neuron_outputs[i], sum);
                        exit(EXIT_FAILURE);
                }
        }
        for (int i = 0; i < neurons_total; i++) {
        		nn->neuron_outputs[i] = temp_neuron_outputs[i];
        }
        for (int i = 0; i < nn->nr_outputs; i++) {
                /* The output neurons are offset by nn->nr_inputs nn->nr_hidden in the neuron_output */
                output[i] = nn->neuron_outputs[nn->nr_inputs + nn->nr_hidden + i];
        }

        //ase_printf("O: %3.2f\n", output[nn->nr_outputs - 1]);
}

void ANN_RandomizeWeights(ANN_t *nn)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        int sign = 0;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j < neurons_total; j++) {
                        nn->weights[i][j] = (float) (rand() % (int) (nn->w_max_init * 100) + (int) nn->w_min_init * 100);
                        nn->weights[i][j] /= 100.0;
                        sign = rand() % 2;
                        if (sign == 1) {
                                nn->weights[i][j] *= -1;
                        }
                }
        }
}

ANN_t *ANN_New(unsigned int nr_inputs,
                unsigned int nr_outputs,
                unsigned int nr_hidden,
                unsigned int nr_max_con_forward,
                unsigned int nr_max_con_recurrent,
                float (*ActivationFunction)(float x),
                float w_max,
                float w_min)
{
        ANN_t *nn = (ANN_t *) MemManager_Xmalloc(sizeof(nn[0]));
        nn->nr_inputs = nr_inputs;
        nn->nr_hidden = nr_hidden;
        nn->nr_outputs = nr_outputs;
        nn->nr_max_con_fw = nr_max_con_forward;
        nn->nr_max_con_rc = nr_max_con_recurrent;
        nn->w_max_init = w_max;
        nn->w_min_init = w_min;
        nn->ActivationFunction = ActivationFunction;

        int neurons_total = nr_inputs + nr_hidden + nr_outputs;
        nn->weights = (float **) MemManager_Xmalloc(sizeof(float *) * neurons_total);
        for (int i = 0; i < neurons_total; i++) {
                nn->weights[i] = (float *) MemManager_Xmalloc(sizeof(float) * neurons_total);
        }

        nn->neuron_outputs = (float *) calloc(sizeof(float), neurons_total);
        ANN_RandomizeWeights(nn);
        if (nr_max_con_forward > 0) {
                if (!ANN_MinimizeNrForwardWeights(nn)) {
                        ase_printf("Could not create a valid network with the specified amount of maximum forward connections\n");
                        ANN_RandomizeWeights(nn);
                }
        }

        if (nr_max_con_recurrent > 0) {
                ANN_MinimizeNrRecurrentWeights(nn);
        }

        return nn;
}

float ANN_FindLargestForwardWeightSmallerThanX(ANN_t *nn, float X)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        float w_max_val = DOUBLE_MIN;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = i; j < neurons_total; j++) {
                        if (nn->weights[i][j] > w_max_val && nn->weights[i][j] < X) {
                                w_max_val = nn->weights[i][j];
                        }
                }
        }

        return w_max_val;
}

float ANN_FindLargestRecurrentWeightSmallerThanX(ANN_t *nn, float X)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        float w_max_val = DOUBLE_MIN;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j <= i; j++) {
                        if (nn->weights[i][j] > w_max_val && nn->weights[i][j] < X) {
                                w_max_val = nn->weights[i][j];
                        }
                }
        }

        return w_max_val;
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ANN_AllInputsLeadToOutput
 *  Description:  Checks whether all input connections reach the output through the
 *                hidden layers
 * =====================================================================================
 */
bool ANN_AllInputsLeadToOutput(ANN_t *nn)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        bool valid_inputs[nn->nr_inputs];
        for (int i = 0; i < nn->nr_inputs; i++) {
                valid_inputs[i] = false;
        }

        /* Check the path for all inputs */
        for (int i = 0; i < nn->nr_inputs; i++) {
                /* For every neuron the input is connected to ..*/
                int k = i;
                for (int j = i; j < neurons_total; j++) {
                        /* Jump through all forward connections until no connections left*/
                        /* Go to the neuron to which the current neuron is connected, if it is not a recurrent connection (not going backwards) */
                        if (nn->weights[k][j] > DOUBLE_MIN) {
                                k = j;
                                //ase_printf("K %i\n", k);
                                if (k > (nn->nr_inputs + nn->nr_hidden - 1)) { /* -1 becuase it is zero indexed! */
                                        valid_inputs[i] = true;
                                }
                        }
                }
        }

        /* Check that all inputs are valid (lead to outputs) */
        for (int i = 0; i < nn->nr_inputs; i++) {
                if (!valid_inputs[i]) {
                        return false;
                }
        }

        return true;
}

void ANN_ScaleHiddenNeuronWeights(ANN_t *nn, float factor)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j < neurons_total; j++) {
                        if (i > nn->nr_inputs - 1 && i < (nn->nr_inputs + nn->nr_hidden)) { /* Hidden neurons. -1 because zero indexed! */
                                nn->weights[i][j] *= factor;
                        } //else { /* Input or Output neurons */

                        //nn->weights[i][j] *= 3;
                        //}
                }
        }
}

void ANN_ScaleAllNeuronWeights(ANN_t *nn, float factor)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j < neurons_total; j++) {
                        nn->weights[i][j] *= factor;
                }
        }
}

void ANN_RandomizeRecurrentWeights(ANN_t *nn)
{
        assert(nn != NULL);

        for (int i = 0; i < (nn->nr_inputs + nn->nr_hidden + nn->nr_outputs); i++) {
                for (int j = 0; j < i; j++) {
                        nn->weights[i][j] = (float) (rand() % (int) (nn->w_max_init * 100) + (int) nn->w_min_init * 100);
                        nn->weights[i][j] /= 100.0;
                }
        }
}

void ANN_RandomizeForwardWeights(ANN_t *nn)
{
        assert(nn != NULL);

        int n_total = (nn->nr_inputs + nn->nr_hidden + nn->nr_outputs);
        for (int i = 0; i < n_total; i++) {
                for (int j = i + 1; j < n_total; j++) {
                        nn->weights[i][j] = (float) (rand() % (int) (nn->w_max_init * 100) + (int) nn->w_min_init * 100);
                        nn->weights[i][j] /= 100;
                        if (rand() % 2 == 1) {
                                nn->weights[i][j] *= -1;
                        }
                }
        }
}

void ANN_MinimizeNrRecurrentWeights(ANN_t *nn)
{
        assert(nn != NULL);

        ANN_RandomizeRecurrentWeights(nn);

        /* Get the NR_MAX_CON_FW largest weight values, in order to determine wich weights to eliminate (set to DOUBLE_MIN) */
        float w_largest[nn->nr_max_con_rc];
        w_largest[0] = ANN_FindLargestRecurrentWeightSmallerThanX(nn, DOUBLE_MAX);
        for (int i = 1; i < nn->nr_max_con_rc; i++) {
                w_largest[i] = ANN_FindLargestRecurrentWeightSmallerThanX(nn, w_largest[i - 1]);
        }

        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j <= i; j++) { /* Only recurrent connections */
                        if (nn->weights[i][j] < w_largest[nn->nr_max_con_rc - 1]) { /* nn->nr_max_con_rc - 1  is the lower bound of weights which should not be deleted! */
                                nn->weights[i][j] = DOUBLE_MIN; // DOUBLE_MIN;
                        }
                }
        }
}

void ANN_DeleteRecurrentConnections(ANN_t *nn)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j <= i; j++) { /* Only recurrent connections */
                        nn->weights[i][j] = DOUBLE_MIN; // DOUBLE_MIN;
                }
        }
}


/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ANN_MinimizeForwardWeights
 *  Description:  Minimizes the nn to the specified (in the ANN_t structure) number of
 *                forward weights.
 *       return:  True if valid network with the specified amount of connections was
 *                was created. False otherwise.
 * =====================================================================================
 */
bool ANN_MinimizeNrForwardWeights(ANN_t *nn)
{
        assert(nn != NULL);

        for (int k = 0; k < 10000; k++) {
                ANN_ScaleHiddenNeuronWeights(nn, (10000 - k) / 10000.0);

                /* Get the NR_MAX_CON_FW largest weight values, in order to determine wich weights to eliminate (set to DOUBLE_MIN) */
                float w_largest[nn->nr_max_con_fw];
                w_largest[0] = ANN_FindLargestForwardWeightSmallerThanX(nn, DOUBLE_MAX);
                for (int i = 1; i < nn->nr_max_con_fw; i++) {
                        w_largest[i] = ANN_FindLargestForwardWeightSmallerThanX(nn, w_largest[i - 1]);
                }

                int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
                for (int i = 0; i < neurons_total; i++) {
                        for (int j = 0; j < neurons_total; j++) {
                                if (nn->weights[i][j] < w_largest[nn->nr_max_con_fw - 1]) { /* nn->nr_max_con_rc - 1  is the lower bound of weights which should not be deleted! */
                                        nn->weights[i][j] = DOUBLE_MIN; // DOUBLE_MIN;
                                }
                        }
                }

                if (!ANN_AllInputsLeadToOutput(nn)) {
                        ANN_RandomizeForwardWeights(nn);
                } else {
                        ANN_RandomizeRecurrentWeights(nn);
                        return true;
                }
        }

        return false;
}

void ANN_MinimizeNrWeights(ANN_t *nn)
{
        assert(nn != NULL);

        ANN_MinimizeNrForwardWeights(nn);
        ANN_MinimizeNrRecurrentWeights(nn);
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  ANN_Copy
 *  Description:  Deep copy
 * =====================================================================================
 */
ANN_t *ANN_Copy(ANN_t *nn)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;

        ANN_t *cpy = (ANN_t *) MemManager_Xmalloc(sizeof(ANN_t));
        memcpy(cpy, nn, sizeof(ANN_t));

        cpy->neuron_outputs = (float *) MemManager_Xmalloc(sizeof(float) * neurons_total);
        memcpy(cpy->neuron_outputs, nn->neuron_outputs, neurons_total);

        cpy->weights = (float **) MemManager_Xmalloc(sizeof(float *) * neurons_total);
        for (int i = 0; i < neurons_total; i++) {
                cpy->weights[i] = (float *) MemManager_Xmalloc(sizeof(float) * neurons_total);
                for (int j = 0; j < neurons_total; j++) {
                        cpy->weights[i][j] = nn->weights[i][j];
                }
        }

        return cpy;
}

void ANN_Delete(void **n)
{
        assert(n != NULL);

        ANN_t **nn = (ANN_t **) n;

        int nt = (*nn)->nr_inputs + (*nn)->nr_outputs + (*nn)->nr_hidden;
        for (int i = 0; i < nt; i++) {
                free((*nn)->weights[i]);
        }

        free((*nn)->weights);
        free((*nn)->neuron_outputs);
        free(*nn);
}

void ANN_Print(ANN_t *nn)
{
        int neurons_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;

        ase_printf("NR OF INPUT NEURONS: %i\n", nn->nr_inputs);
        ase_printf("NR OF OUTPUT NEURONS: %i\n", nn->nr_outputs);
        ase_printf("NR OF HIDDEN NEURONS: %i\n", nn->nr_hidden);
        ase_printf("MAX NR OF FORWARD CONNECTIONS: %i\n", nn->nr_max_con_fw);
        ase_printf("MAX NR OF RECURRENT CONNECTIONS: %i\n", nn->nr_max_con_rc);
        for (int i = 0; i < nn->nr_outputs; i++) {
                ase_printf("LAST OUTPUT%i: %3.2f\n", i, nn->neuron_outputs[nn->nr_inputs + nn->nr_hidden + i]);
                assert(nn->neuron_outputs[nn->nr_inputs + nn->nr_hidden + i] < 2);
        }

        for (int i = 0; i < neurons_total; i++) {
                for (int j = 0; j < neurons_total; j++) {
                        if (nn->weights[i][j] == DOUBLE_MIN) {
                                ase_printf("NA\t");
                        } else {
                                ase_printf("%3.2f\t", nn->weights[i][j]);
                        }
                }

                ase_printf("\n");
        }
}

void ANN_ClearConnections(ANN_t *nn)
{
        int n_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        for (int i = 0; i < n_total; i++) {
                for (int j = 0; j < n_total; j++) {
                        nn->weights[i][j] = DOUBLE_MIN;
                }
        }
}

void ANN_Connect(ANN_t *nn, unsigned int n1, unsigned int n2, float w)
{
        int n_total = nn->nr_inputs + nn->nr_hidden + nn->nr_outputs;
        assert(n1 < n_total);
        assert(n2 < n_total);

        nn->weights[n1][n2] = w;
}


/*-----------------------------------------------------------------------------
 *  DEFAULT NN MUTATION AND CROSSOVER FUNCTIONS
 *-----------------------------------------------------------------------------*/
void ANN_Mutate(ANN_t *nn, float probability)
{
        int n_tot = (nn->nr_inputs + nn->nr_hidden + nn->nr_outputs);
        //#define ANN_Mutate_PROB rand() % (int)((1 / 0.005)// probability))
        #define ANN_Mutate_PROB rand() % (int)((1 / probability))
        for (int i = 0; i < n_tot; i++) {
                for (int j = 0; j < n_tot; j++) {
                        if (nn->weights[i][j] > DOUBLE_MIN) {
                                if (ANN_Mutate_PROB == 0) {
                                        ANN_ResetNeuronState(nn);
                                        switch (rand() % 22) {
                                                case 0:
                                                        nn->weights[i][j] += ((rand() % 100) + 1) / 1000.0;
                                                        break;
                                                case 1:
                                                        nn->weights[i][j] -= ((rand() % 100) + 1) / 1000.0;
                                                        break;
                                                case 2:
                                                        nn->weights[i][j] = ((rand() % 100) + 1) / 1000.0;
                                                        break;
                                                case 3:
                                                        nn->weights[i][j] *= ((rand() % 100) + 1) / 1000.0;
                                                        break;
                                                case 5:
                                                        nn->weights[i][j] *= -1;
                                                        break;
                                                case 6:
                                                        nn->weights[i][j] = ((rand() % 1000) + 1) / 100.0;
                                                        break;
                                                case 7:
                                                        nn->weights[i][j] -= ((rand() % 1000) + 1) / 100.0;
                                                        break;
                                                case 8:
                                                        nn->weights[i][j] /= 1000;
                                                        break;
                                                case 9: 
                                                        nn->weights[i][j] /= 100.0;
                                                        break;
                                                case 10:
                                                        nn->weights[i][j] /= 10.0;
                                                        break;
                                                case 11: 
                                                        nn->weights[i][j] = 0.0;
                                                        break;
                                                case 12:
                                                        nn->weights[i][j] *= (rand() % 1000) / 1000.0;
                                                        break;
                                                case 13:
                                                        nn->weights[i][j] /= (rand() % 1000) / 1000.0;
                                                        break;
                                                case 14:
                                                        nn->weights[i][j] += (rand() % 1000) / 1000.0;
                                                        break;
                                                case 15:
                                                        nn->weights[i][j] -= (rand() % 1000) / 1000.0;
                                                        break;
                                                case 16:
                                                        nn->weights[i][j] *= 0.9;
                                                        break;
                                                case 17:
                                                        nn->weights[i][j] *= 1.1;
                                                        break; 
                                                case 18:
                                                        nn->weights[i][j] += ((rand() % 100) + 1) / 10000.0;
                                                        break;
                                                case 19:
                                                        nn->weights[i][j] -= ((rand() % 100) + 1) / 10000.0;
                                                        break;
                                                case 20:
                                                        nn->weights[i][j] += ((rand() % 100) + 1) / 100000.0;
                                                        break;
                                                case 21:
                                                        nn->weights[i][j] -= ((rand() % 100) + 1) / 100000.0;
                                                        break;

                                                default:
                                                        break;
                                        }
                                }
                        }
                }
        }
}

ANN_t *ANN_Crossover(ANN_t *nn1, ANN_t *nn2)
{
        int n_total = nn1->nr_inputs + nn1->nr_hidden + nn1->nr_outputs;
        ANN_t *cpy = NULL;
        if (rand() % 10 < 6) /*  Do the crossover */
        {
                switch (rand() % 3) {
                case 0:
                        cpy = ANN_Copy(nn1);
                        for (int i = 0; i < n_total / (rand() % n_total + 1); i++) {
                                for (int j = 0; j < n_total; j++) {
                                        if (nn2->weights[i][j] > DOUBLE_MIN) {
                                                cpy->weights[i][j] = nn2->weights[i][j];
                                        }
                                }
                        }
                break;
                case 1:
                        cpy = ANN_Copy(nn2);
                        for (int i = 0; i < n_total / (rand() % n_total + 1); i++) {
                                for (int j = 0; j < n_total; j++) {
                                        if (nn1->weights[i][j] > DOUBLE_MIN) {
                                                cpy->weights[i][j] = nn1->weights[i][j];
                                        }
                                }
                        }
                break;
                case 2:
                        cpy = ANN_Copy(nn1);
                        for (int i = 0; i < n_total; i++) {
                                for (int j = 0; j < (rand() % n_total + 1); j++) {
                                        if (nn2->weights[i][j] > DOUBLE_MIN) {
                                                cpy->weights[i][j] = nn2->weights[i][j];
                                        }
                                }
                        }
                break;
                default:
                break;
                }
        } else {
                if (rand() % 2 == 0)
                        cpy = ANN_Copy(nn2);
                else
                        cpy = ANN_Copy(nn1);
        }

        ANN_ResetNeuronState(cpy);
        return cpy;
}


