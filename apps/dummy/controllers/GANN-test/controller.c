#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/GA/ga.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/infrastructure/MemManager/MemManager.h>

float GANN_Fitness(GA_individual_t *indv)
{
        float error_sum = 0.0;
        float inputs = 0.0;
        float outputs = 0.0;
        float period = 2 * PI;
        ANN_t *ann = (ANN_t*)indv->indv;
        ANN_ResetNeuronState(ann);
        
        for (float i = 0; i < 1; i = i + 0.01) {
                inputs = i;
                ANN_Execute(ann, &inputs, &outputs);
                error_sum += fabs(sin(inputs * period) - outputs * period);
                //error_sum += fabs(sin(inputs * period));
        }
        //printf("error_sum %3.2f\n", error_sum);
        //exit(0);
        return (1500 - error_sum);
}

GA_individual_t *GANN_Clone(GA_individual_t *indv)
{
        ANN_t *cpy = ANN_Copy(indv->indv);
        ANN_ResetNeuronState(cpy);
        return GA_New(cpy);
}

bool GANN_Done(GA_individual_t *indv)
{
        if (indv->fit >= 1499.85)
                return true;
        return false;
}

GA_individual_t *GANN_Reproduce(GA_individual_t *parent1, GA_individual_t *parent2)
{
        ANN_t *ar = ANN_Crossover(parent1->indv, parent2->indv);
        ANN_Mutate(ar, 0.3);
        return GA_New(ar);
}

void GANN_Delete(GA_individual_t *indv)
{
        ANN_Delete(&indv->indv);
        GA_Delete(indv);
}       

/* The genetic algorithms goal is set to get a float with value 100 (by mutation and reproduction) */
void controller_init() 
{
        int pop_size = 400;
        GA_individual_t *population[pop_size];
        
        for (int i = 0; i < pop_size; i++) {
                population[i] = GA_New(ANN_New(1, 1, 2, 0, 0, &ANN_Tanh, 200.0, 0.01));
                ANN_RandomizeWeights(population[i]->indv);
        }

        int cnt = 0;
        do {
                GA_GeneticAlgorithm(population,
                                    pop_size,
                                    GANN_Fitness,
                                    GANN_Reproduce,
                                    GANN_Delete,
                                    GANN_Clone,
                                    true);
                if (cnt % 100 == 0)
                        ase_printf("Generation: %i\tFitness: %3.2f\n", cnt, population[0]->fit); 
                cnt++;
        } while (!GANN_Done(population[0]));
        ANN_t *ann = (ANN_t*)population[0]->indv;
        float inputs = 0.0;
        float outputs = 0.0;
        for (float i = 0; i < 1; i = i + 0.01) {
                inputs = i;
                ANN_Execute(ann, &inputs, &outputs);
                outputs *= 2 * PI;
                ase_printf("Input: %3.2f\tOutput: %3.2f\tSin: %3.2f\n", inputs, outputs, sin(i * 2 * PI));
        }
        exit(0);
}
