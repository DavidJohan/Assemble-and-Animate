#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/infrastructure/MemManager/MemManager.h>

typedef struct PSO_Parameters_s {
        unsigned int pc;
        float *pv;
} PSO_Parameters_t;

typedef struct PSO_Particle_s {
        PSO_Parameters_t params;
        PSO_Parameters_t best_params;
        PSO_Parameters_t velocity;
        float fitness;
        float best_fitnesss;
} PSO_Particle_t;

typedef struct PSO_SwarmParams_s {
        float alpha;
        float beta;
        float gamma;
        float delta;
        float epsilon;
        int size;
} PSO_SwarmParams_t;

PSO_Parameters_t *PSO_NewParameters(size_t nr_of_parameters)
{
        assert(nr_of_parameters > 0);

        PSO_Parameters_t *new = (PSO_Parameters_t*)MemManager_Xmalloc(sizeof(PSO_Parameters_t));        
        new->pv = (float*)MemManager_Xmalloc(sizeof(float) * nr_of_parameters);
        new->pc = nr_of_parameters;
        for (int i = 0; i < new->pc; i++) 
                new->pv[i] = 0.0;
}

PSO_Particle_s *PSO_NewParticle(PSO_Parameters_t *param)
{
        PSO_Particle_t *new = (PSO_Particle_t*)MemManager_Xmalloc(sizeof(PSO_Particle_t));
        new->params = param;
        new->best_params = PSO_CloneParameters(param);
        new->velocity = PSO_NewParameters(param->pc);
        new->fitness = DOUBLE_MIN;
        new->best_fitness = DOUBLE_MIN;
}

PSO_Parameters_t *PSO_CloneParameters(PSO_Parameters_t *param)
{
        assert(param != NULL);

        PSO_Parameters_t *new = PSO_NewParameters(param->pc);        
        memcpy(new->pv, param->pv, sizeof(float) * param->pc);
}

void PSO_DeleteParameter(PSO_Parameters_t **param)
{
        free((*param)->pv);
        free(param);
}

#if 0
void PSO_DeleteSwarm(PSO_Swarm_t *swarm)
{
        free(swarm);
}
#endif 

void PSO_DeleteParticle(PSO_Particle_t **particle)
{
        PSO_DeleteParameter((*particle)->location);
        PSO_DeleteParameter((*particle)->best_location);
        free(particle);
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
        for (int i = 0; i < A->pc; i++) {
                A->pv[i] = A->pv[i] - B->pv[i];
        }

}

/* A = A * f */
void PSO_ParametersMultiplyWithFactor(PSO_Parameter_t *A, float f)
{
        for (int i = 0; i < A->pc; i++) {
                A->pv[i] *= f;
        }
}

PSO_Parameter_t *PSO(PSO_SwarmParams_t *swarm_params,
                     PSO_Particle_t *particles[])
{
        for (int i = 0; i < swarm_params->size; i++) {
                PSO_Params_t *prev_fittest_particle_params = PSO_CloneParameters(particles[i]->best_location);
                PSO_Params_t *prev_fittest_informant_params = PSO_CloneParameters(PSO_GetPreviousFittestInformantParams(particles));
                PSO_Params_t *prev_fittest_of_all_params  = PSO_CloneParameters(PSO_GetPreviousFittestOfAll(particles));
                float b = (rand() % (int) 10000.0 * swarm_params->beta) / 10000.0;
                float c = (rand() % (int) 10000.0 * swarm_params->gamma) / 10000.0;
                float d = (rand() % (int) 10000.0 * swarm_params->delta) / 10000.0;
                
                PSO_ParametersSubtractBFromA(prev_fittest_particle_params, particles[i]->params);
                PSO_ParametersMultiplyWithFactor(prev_fittest_particle_params, b);

                PSO_ParametersSubtractBFromA(prev_fittest_informant_params, particles[i]->params);
                PSO_ParametersMultiplyWithFactor(prev_fittest_informant_params, c);

                PSO_ParametersSubtractBFromA(prev_fittest_of_all_params, particles[i]->params);
                PSO_ParametersMultiplyWithFactor(prev_fittest_of_all_params, d);

                PSO_ParametersMultiplyWithFactor(&particles[i]->velocity, swarm_param->alpha);
                PSO_ParametersAdd(&particles[i]->velocity, prev_fittest_particle_params);
                PSO_ParametersAdd(&particles[i]->velocity, prev_fittest_informant_params);
                PSO_ParametersAdd(&particles[i]->velocity, prev_fittest_of_all_params);

                PSO_ParametersAdd(&particles[i]->params, &particles[i]->velocity);
                
                

        }
}

/*-----------------------------------------------------------------------------
 *  ANN SPECIFIC CODE!
 *-----------------------------------------------------------------------------*/
void PSOANN_GenoToPheno(ANN_t *ann, PSO_Parameters_t *params)
{
        assert(ann != NULL);
        assert(params != NULL);
        
        n_neurons = ann->nr_hidden + ann->nr_inputs + ann->nr_outputs;
        for (int i = 0; i < n_neurons; i++) {
                for (int j = 0; j < n_neurons; j++) {
                        ann->weights[i][j] = params->pv[i * n_neurons + j];
                }
        }

}

PSO_Parameters_t *PSOANN_PhenoToGeno(ANN_t *ann)
{
        assert(ann != NULL);

        n_neurons = ann->nr_hidden + ann->nr_inputs + ann->nr_outputs;
        n_params = n_neurons * n_neurons;
        PSO_Parameters_t new_params = PSO_NewParameters(n_params);
        for (int i = 0; i < n_neurons; i++) {
                for (int j = 0; j < n_neurons; j++) {
                        new_params->pv[i * n_neurons + j] = ann->weights[i][j];
                }
        }
        return new_params;
}

float PSOANN_Fitness(PSO_Parameters_t *params)
{
        static ann = ANN_New(1, 1, 3, 0, 0, &ANN_Sigmoid, 1, 0);
        PSOANN_GenoToPheno(ann, params);
        /* Evealuate ann */
}

/* The genetic algorithms goal is set to get a float with value 100 (by mutation and reproduction) */
void controller_init() 
{
        exit(0);
}
