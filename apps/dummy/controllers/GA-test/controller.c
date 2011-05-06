#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/GA/ga.h>
#include <ase/infrastructure/MemManager/MemManager.h>

float GADemo_Fitness(GA_individual_t *indv)
{
        float val = *(float*)(indv->indv);
        if (val < 0) 
                return 0;
        if (val > 100)
                return (100 - ((int)val % 100));
        return val; 

}

GA_individual_t *GADemo_Clone(GA_individual_t *indv)
{
        GA_individual_t *new  = (GA_individual_t*)MemManager_Xmalloc(sizeof(GA_individual_t));
        new->indv = (float*)MemManager_Xmalloc(sizeof(float));
        *(float*)new->indv = *(float*)(indv->indv);
        return new;
}

bool GADemo_Done(GA_individual_t *indv)
{
        if ((int)*(float*)indv->indv == 100)
                return true;
        return false;
}

GA_individual_t *GADemo_Reproduce(GA_individual_t *parent1, GA_individual_t *parent2)
{
        GA_individual_t *new = (GA_individual_t*)MemManager_Xmalloc(sizeof(GA_individual_t));
        new->indv = (float*)MemManager_Xmalloc(sizeof(float));
        *(float*)new->indv = (*(float*)(parent1->indv) + *(float*)(parent2->indv)) / 2.0;

        /* mutate */
        if (rand() % 500 == 0) {
                if (rand() % 2 == 0)
                        *(float*)new->indv *= 0.8;
                else
                        *(float*)new->indv *= 1.2;
        }
        return new;
}

void GADemo_Delete(GA_individual_t *indv)
{
        free(indv->indv);
        free(indv);
}       

/* The genetic algorithms goal is set to get a float with value 100 (by mutation and reproduction) */
void controller_init() 
{
        int pop_size = 1000;
        float *randoms[pop_size];
        GA_individual_t *population[pop_size];
        for (int i = 0; i < pop_size; i++) {
                randoms[i] = (float*)MemManager_Xmalloc(sizeof(float));
                *randoms[i] = rand() % 10;
                population[i] = GA_New(randoms[i]);
        }

        int cnt = 0;
        do {
                GA_GeneticAlgorithm(population,
                                    pop_size,
                                    GADemo_Fitness,
                                    GADemo_Reproduce,
                                    GADemo_Delete,
                                    GADemo_Clone,
                                    true);
                ase_printf("Generation: %i\tFitness: %3.2f\tValue: %3.2f\n", cnt, GADemo_Fitness(population[0]), *(float*)population[0]->indv); 
                cnt++;
        } while (!GADemo_Done(population[0]));
        exit(0);
}
