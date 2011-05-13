/*
 * =====================================================================================
 *
 *       Filename:  pso.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2011-05-13 13:50:17
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Mikael Moghadam (mimog), binarybusiness [at] gmail [dot] com
 *        Company:  
 *        License:
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
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/control/strategies/PSO/pso.h>

PSO_Parameters_t *PSO_NewParameters(size_t nr_of_parameters)
{
        assert(nr_of_parameters > 0);

        PSO_Parameters_t *new = (PSO_Parameters_t*)MemManager_Xmalloc(sizeof(PSO_Parameters_t));        
        new->pv = (float*)MemManager_Xmalloc(sizeof(float) * nr_of_parameters);
        new->pc = nr_of_parameters;
        for (int i = 0; i < new->pc; i++) 
                new->pv[i] = 0.0;
        return new;
}

PSO_Parameters_t *PSO_CloneParameters(PSO_Parameters_t *param)
{
        assert(param != NULL);

        PSO_Parameters_t *new = PSO_NewParameters(param->pc);        
        memcpy(new->pv, param->pv, sizeof(float) * param->pc);
        return new;
}

PSO_Particle_t *PSO_NewParticle(PSO_Parameters_t *param)
{
        PSO_Particle_t *new = (PSO_Particle_t*)MemManager_Xmalloc(sizeof(PSO_Particle_t));
        new->params = param;
        new->best_params = PSO_CloneParameters(param);
        new->velocity = PSO_NewParameters(param->pc);
        new->fitness = DOUBLE_MIN;
        new->best_fitness = DOUBLE_MIN;
        new->no_fit_change_cnt = 0;
        return new;
}

void PSO_DeleteParameter(PSO_Parameters_t **param)
{
        free((*param)->pv);
        free(*param);
}

#if 0
void PSO_DeleteSwarm(PSO_Swarm_t *swarm)
{
        free(swarm);
}
#endif 

void PSO_DeleteParticle(PSO_Particle_t **particle)
{
        PSO_DeleteParameter(&(*particle)->params);
        PSO_DeleteParameter(&(*particle)->best_params);
        PSO_DeleteParameter(&(*particle)->velocity);
        free(*particle);
}

/* A = A + B */
void PSO_ParametersAdd(PSO_Parameters_t *A, PSO_Parameters_t *B)
{
        for (int i = 0; i < A->pc; i++) {
                A->pv[i] = A->pv[i] + B->pv[i];
        }
}

/* A = A - B */
void PSO_ParametersSubtractBFromA(PSO_Parameters_t *A, PSO_Parameters_t *B)
{
        assert(A != NULL);
        assert(B != NULL);

        for (int i = 0; i < A->pc; i++) {
                A->pv[i] = A->pv[i] - B->pv[i];
        }
}

/* A = B - A */
void PSO_ParametersSubtractAFromB(PSO_Parameters_t *A, PSO_Parameters_t *B)
{
        assert(A != NULL);
        assert(B != NULL);

        for (int i = 0; i < A->pc; i++) {
                A->pv[i] = B->pv[i] - A->pv[i];
        }
}

/* A = A * f */
void PSO_ParametersMultiplyWithFactor(PSO_Parameters_t *A, float f)
{
        assert(A != NULL);

        for (int i = 0; i < A->pc; i++) {
                A->pv[i] *= f;
        }
}

PSO_Particle_t *PSO_GetFittestOfAll(PSO_SwarmParams_t *swarm_params, PSO_Particle_t *particles[], float (*Fitness)(PSO_Parameters_t*))
{
        assert(swarm_params != NULL);
        assert(particles != NULL);

        PSO_Particle_t *best = particles[0];
        for (int i = 0; i < swarm_params->size; i++) {
                if (particles[i]->fitness == DOUBLE_MIN) {
                        particles[i]->fitness = Fitness(particles[i]->params);
                        particles[i]->best_fitness = particles[i]->best_fitness;
                        for (int j = 0; j < particles[i]->best_params->pc; j++)
                                particles[i]->best_params->pv[j] = particles[i]->params->pv[j];
                }
                if (best->best_fitness < particles[i]->best_fitness) {
                        best = particles[i];
                }
        }
        return best;
}

PSO_Particle_t *PSO_GetFittestOfInformants(PSO_SwarmParams_t *swarm_params, PSO_Particle_t *particles[], float (*Fitness)(PSO_Parameters_t*))
{
        assert(swarm_params != NULL);
        assert(particles != NULL);

        PSO_Particle_t *best = particles[0];
        int visited[swarm_params->nr_informants - 1];
        visited[0] = 0;
        for (int i = 1; i < swarm_params->nr_informants; i++) {
                visited[i] = rand() % swarm_params->size;
        }

        for (int i = 0; i < swarm_params->nr_informants; i++) {
                if (particles[visited[i]]->fitness == DOUBLE_MIN) {
                        particles[visited[i]]->fitness = Fitness(particles[visited[i]]->params);
                        particles[visited[i]]->best_fitness = particles[visited[i]]->best_fitness;
                        for (int j = 0; j < particles[visited[i]]->best_params->pc; j++)
                                particles[visited[i]]->best_params->pv[j] = particles[visited[i]]->params->pv[j];
                }
                if (best->best_fitness < particles[visited[i]]->best_fitness) {
                        best = particles[i];
                }
        }
        return best;
}

void PSO(PSO_SwarmParams_t *swarm_params,
                     PSO_Particle_t *particles[],
                     float (*Fitness)(PSO_Parameters_t*),
                     float parameter_max,
                     float parameter_min,
                     PSO_Particle_t **best)
{
        assert(swarm_params != NULL);
        assert(Fitness != NULL);
        assert(best != NULL);

        if ((*best)->best_fitness == DOUBLE_MIN)
                (*best) = PSO_GetFittestOfAll(swarm_params, particles, Fitness);

        for (int i = 0; i < swarm_params->size; i++) {
                PSO_Parameters_t *fittest_particle_params = PSO_CloneParameters(particles[i]->best_params);
                //PSO_Parameters_t *fittest_of_all_params  = PSO_CloneParameters(PSO_GetFittestOfAll(swarm_params, particles, Fitness)->best_params);
                PSO_Parameters_t *fittest_of_all_params  = PSO_CloneParameters((*best)->best_params);//PSO_CloneParameters(PSO_GetFittestOfAll(swarm_params, particles, Fitness)->best_params);
                //PSO_Parameters_t *fittest_of_informants  = PSO_CloneParameters(PSO_GetFittestOfInformants(swarm_params, particles, Fitness)->best_params);
                float b = (rand() % (int) 1000000.0 * swarm_params->acceleration_local) / 1000000.0;
                float c = (rand() % (int) 1000000.0 * swarm_params->acceleration_global) / 1000000.0;
                //float d = (rand() % (int) 10000.0 * swarm_params->acceleration_random) / 10000.0;
                //ase_printf("b: %3.4f\nc: %3.4f\nd: %3.4f\n", b, c, d);
                //exit(0);
                PSO_ParametersSubtractBFromA(fittest_particle_params, particles[i]->params);
                PSO_ParametersMultiplyWithFactor(fittest_particle_params, b);

                PSO_ParametersSubtractBFromA(fittest_of_all_params, particles[i]->params);
                PSO_ParametersMultiplyWithFactor(fittest_of_all_params, c);

                //PSO_ParametersSubtractBFromA(fittest_of_informants, particles[i]->params);
                //PSO_ParametersMultiplyWithFactor(fittest_of_informants, d);

                PSO_ParametersMultiplyWithFactor(particles[i]->velocity, swarm_params->inertia);
                PSO_ParametersAdd(particles[i]->velocity, fittest_particle_params);
                PSO_ParametersAdd(particles[i]->velocity, fittest_of_all_params);
                //PSO_ParametersAdd(particles[i]->velocity, fittest_of_informants);

                PSO_ParametersAdd(particles[i]->params, particles[i]->velocity);
                
                if (particles[i]->no_fit_change_cnt > 5000) {//&& particles[i]->fitness < 0.5) {
                        //ase_printf("Particle %i parameters stuck. Randomizing!\n", i);
                        /* Randomize parameters within specified intervals */
                        for (int j = 0; j < particles[i]->params->pc; j++) {
                                particles[i]->params->pv[j] = ((rand() % (int)(100000.0  * (fabs(parameter_max) + fabs(parameter_min)))) / 100000.0) - parameter_min;
                        }
                }
                float fitness = Fitness(particles[i]->params);
                if (fabs(particles[i]->fitness - fitness) < 0.0001)
                        particles[i]->no_fit_change_cnt++;
                else 
                        particles[i]->no_fit_change_cnt = 0;

                particles[i]->fitness = fitness;
                if (particles[i]->fitness  > particles[i]->best_fitness) {
                        particles[i]->best_fitness = particles[i]->fitness;
                        for (int j = 0; j < particles[i]->params->pc; j++) {
                                particles[i]->best_params->pv[j] = particles[i]->params->pv[j];
                        }
                }

                if (particles[i]->best_fitness > (*best)->best_fitness) {
                       (*best) = particles[i];
                }

                PSO_DeleteParameter(&fittest_particle_params);
                PSO_DeleteParameter(&fittest_of_all_params);
                //PSO_DeleteParameter(&fittest_of_informants);
        }
}


