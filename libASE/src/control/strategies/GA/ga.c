/*
 * =====================================================================================
 *
 *       Filename:  ga.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-05-06 12:09:21
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mikael Moghadam (mimog), binarybusiness [at] gmail [dot] com
 *        Company:  
 *        License:
 *
 * =====================================================================================
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <assert.h>
#include <ase/control/strategies/GA/ga.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/targets/AbstractModuleApi.h>

GA_individual_t *GA_New(void *data)
{
        GA_individual_t *new = (GA_individual_t *) MemManager_Xmalloc(sizeof(GA_individual_t));
        new->indv = data;
        new->fit = DOUBLE_MIN;
        return new;
}

void GA_Delete(GA_individual_t *indv)
{
        free(indv);
}

void GA_ClearFitness(GA_individual_t *indv)
{
        indv->fit = DOUBLE_MIN;
}

int GA_Cmp(const void *e1, const void *e2)
{
        GA_individual_t *g1 = *(GA_individual_t **) e1;
        GA_individual_t *g2 = *(GA_individual_t **) e2;
        float dif = (g2->fit - g1->fit);
        if (dif < 0) {
                return -1;
        }

        if (dif > 0) {
                return 1;
        }

        return 0;
}

float GA_DecoratedFitness(GA_individual_t *indv, float (*GA_FitnessFunc)(GA_individual_t *ind))
{
        if (indv->fit > DOUBLE_MIN) {
                //ase_printf("BOM!\n");
                return indv->fit;
        } else {
                //ase_printf("XXXXX\n");
                indv->fit = GA_FitnessFunc(indv); 
                return indv->fit;
        }
}

/*-----------------------------------------------------------------------------
 * Insertion sort. It is fast for small sorts, and especially if moving around
 * small elements, which we are (pointers).
 *-----------------------------------------------------------------------------*/
void GA_Sort(GA_individual_t *population[], int pop_size, float (*GA_FitnessFunc)(GA_individual_t *ind))
{
        for (int i = 0; i < pop_size; i++) {
                population[i]->fit = GA_DecoratedFitness(population[i], GA_FitnessFunc);
        }

        qsort(population, pop_size, sizeof(GA_individual_t*), GA_Cmp);
}

GA_individual_t *GA_SelectParentRoulette(GA_individual_t *population[], int pop_size, float (*GA_Fit)(GA_individual_t *ind))
{
        float fit_sum = 0;
        float rnd_1 = 0;
        for (int i = 0; i < pop_size; i++) {
                fit_sum += GA_DecoratedFitness(population[i], GA_Fit);
        }

        rnd_1 = ((rand() % 1000) * fit_sum) / 1000.0;
        fit_sum = 0;
        for (int i = 0; i < pop_size; i++) {
                fit_sum += GA_DecoratedFitness(population[i], GA_Fit);
                if (fit_sum >= rnd_1) {
                        return population[i];
                }
        }

        assert(false);
        return population[0];
}

GA_individual_t *GA_GeneticAlgorithm(GA_individual_t *population[],
                int pop_size,
                float (*GA_FitnessFunc)(GA_individual_t *individual),
                GA_individual_t *(*GA_Reproduce)(GA_individual_t *parent1, GA_individual_t *parent2),
                void (*GA_Delete)(GA_individual_t *individual),
                GA_individual_t *(*GA_Clone)(GA_individual_t *individual),
                bool elitism)
{
        GA_individual_t *parent_1;
        GA_individual_t *parent_2;
        GA_individual_t *pop_new[pop_size];
        int n_elite = 0;
        static init = 0;
        if (init == 0) {
                GA_Sort(population, pop_size, GA_FitnessFunc);
                init = 1;
        }
#if 1
        if (elitism) {
                n_elite = pop_size / 10;
                if (n_elite > 20) 
                        n_elite = 20;
        } else {
                n_elite = 0;
        }
/*
        for (int i = 0; i < n_elite; i++) {
                pop_new[i] = population[i];
        }
*/
        for (int i = n_elite; i < pop_size; i++) {
                parent_1 = GA_SelectParentRoulette(population, pop_size, GA_FitnessFunc);
                parent_2 = GA_SelectParentRoulette(population, pop_size, GA_FitnessFunc);
                pop_new[i] = GA_Reproduce(parent_1, parent_2);
                GA_ClearFitness(pop_new[i]);
        }

        for (int i = n_elite; i < pop_size; i++) {
                GA_Delete(population[i]);
                population[i] = pop_new[i];
        }
#endif
        GA_Sort(population, pop_size, GA_FitnessFunc);
        return population[0];
}
