/*
 * =====================================================================================
 *
 *       Filename:  ga.h
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

#ifndef ASE_GA_H
#define ASE_GA_H

#include <stdbool.h>
#include <float.h>

#define DOUBLE_MIN (-(FLT_MAX / 2))
#define DOUBLE_MAX (FLT_MAX / 2)
#define PI 3.14159265

typedef struct GA_individual_s {
        void *indv;
        float fit;
} GA_individual_t;

GA_individual_t *GA_New(void *data);
void GA_Delete(GA_individual_t *indv);
GA_individual_t *GA_GeneticAlgorithm(GA_individual_t *population[],
                int pop_size,
                float (*GA_FitnessFunc)(GA_individual_t *individual),
                GA_individual_t *(*GA_Reproduce)(GA_individual_t *parent1, GA_individual_t *parent2),
                void (*GA_Delete)(GA_individual_t *individual),
                GA_individual_t *(*GA_Clone)(GA_individual_t *individual),
                bool elitism);

#endif
