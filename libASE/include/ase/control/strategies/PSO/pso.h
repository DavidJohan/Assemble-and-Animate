/*
 * =====================================================================================
 *
 *       Filename:  pso.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-05-13 13:46:48
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mikael Moghadam (mimog), binarybusiness [at] gmail [dot] com
 *        Company:  
 *        License:
 *
 * =====================================================================================
 */
#ifndef ASE_PSO_H
#define ASE_PSO_H

#include <stddef.h>
#include <float.h>

#ifndef DOUBLE_MIN
#define DOUBLE_MIN -(FLT_MAX/2)
#endif

typedef struct PSO_Parameters_s {
        unsigned int pc;
        float *pv;
} PSO_Parameters_t;

typedef struct PSO_Particle_s {
        PSO_Parameters_t *params;
        PSO_Parameters_t *best_params;
        PSO_Parameters_t *velocity;
        float fitness;
        int no_fit_change_cnt;
        float best_fitness;
} PSO_Particle_t;

typedef struct PSO_SwarmParams_s {
        float inertia;
        float acceleration_local;
        float acceleration_global;
        float acceleration_random;
        int size;
        int nr_informants;
} PSO_SwarmParams_t;

PSO_Parameters_t *PSO_NewParameters(size_t nr_of_parameters);
PSO_Particle_t *PSO_NewParticle(PSO_Parameters_t *param);
void PSO_DeleteParticle(PSO_Particle_t **particle);
void PSO_DeleteParameter(PSO_Parameters_t **param);
PSO_Parameters_t *PSO_CloneParameters(PSO_Parameters_t *param);
void PSO(PSO_SwarmParams_t *swarm_params,
                     PSO_Particle_t *particles[],
                     float (*Fitness)(PSO_Parameters_t*),
                     PSO_Particle_t **best);
PSO_Particle_t *PSO_GetFittestOfAll(PSO_SwarmParams_t *swarm_params, PSO_Particle_t *particles[], float (*Fitness)(PSO_Parameters_t*));
#endif
