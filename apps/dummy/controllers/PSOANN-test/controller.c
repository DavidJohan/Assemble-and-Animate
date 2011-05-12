#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <float.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/infrastructure/MemManager/MemManager.h>

#define PI 3.14159265

typedef struct PSO_Parameters_s {
        unsigned int pc;
        float *pv;
} PSO_Parameters_t;

typedef struct PSO_Particle_s {
        PSO_Parameters_t *params;
        PSO_Parameters_t *best_params;
        PSO_Parameters_t *velocity;
        float fitness;
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
        for (int i = 0; i < A->pc; i++) {
                A->pv[i] = A->pv[i] - B->pv[i];
        }
}

/* A = A * f */
void PSO_ParametersMultiplyWithFactor(PSO_Parameters_t *A, float f)
{
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

PSO_Particle_t *PSO(PSO_SwarmParams_t *swarm_params,
                     PSO_Particle_t *particles[],
                     float (*Fitness)(PSO_Parameters_t*))
{
        for (int i = 0; i < swarm_params->size; i++) {
                PSO_Parameters_t *fittest_particle_params = PSO_CloneParameters(particles[i]->best_params);
                PSO_Parameters_t *fittest_of_all_params  = PSO_CloneParameters(PSO_GetFittestOfAll(swarm_params, particles, Fitness)->best_params);
                //PSO_Parameters_t *fittest_of_informants  = PSO_CloneParameters(PSO_GetFittestOfInformants(swarm_params, particles, Fitness)->best_params);
                float b = (rand() % (int) 10000.0 * swarm_params->acceleration_local) / 10000.0;
                float c = (rand() % (int) 10000.0 * swarm_params->acceleration_global) / 10000.0;
                float d = (rand() % (int) 10000.0 * swarm_params->acceleration_random) / 10000.0;
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
                        
                float fitness = Fitness(particles[i]->params);
                particles[i]->fitness = fitness;
                if (particles[i]->fitness  > particles[i]->best_fitness) {
                        particles[i]->best_fitness = particles[i]->fitness;
                        for (int j = 0; j < particles[i]->params->pc; j++) {
                                particles[i]->best_params->pv[j] = particles[i]->params->pv[j];
                        }
                }
                PSO_DeleteParameter(&fittest_particle_params);
                PSO_DeleteParameter(&fittest_of_all_params);
                PSO_DeleteParameter(&fittest_of_informants);
        }
        return PSO_GetFittestOfAll(swarm_params, particles, Fitness);
}

/*-----------------------------------------------------------------------------
 *  ANN SPECIFIC CODE!
 *-----------------------------------------------------------------------------*/
void PSOANN_GenoToPheno(ANN_t *ann, PSO_Parameters_t *params)
{
        assert(ann != NULL);
        assert(params != NULL);
        
        int n_neurons = ann->nr_hidden + ann->nr_inputs + ann->nr_outputs;
        for (int i = 0; i < n_neurons; i++) {
                for (int j = 0; j < n_neurons; j++) {
                        ann->weights[i][j] = params->pv[i * n_neurons + j];
                }
        }

}½

PSO_Parameters_t *PSOANN_PhenoToGeno(ANN_t *ann)
{
        assert(ann != NULL);

        int n_neurons = ann->nr_hidden + ann->nr_inputs + ann->nr_outputs;
        int n_params = n_neurons * n_neurons;
        PSO_Parameters_t *new_params = PSO_NewParameters(n_params);
        for (int i = 0; i < n_neurons; i++) {
                for (int j = 0; j < n_neurons; j++) {
                        new_params->pv[i * n_neurons + j] = ann->weights[i][j];
                }
        }
        return new_params;
}

float PSOANN_Fitness(PSO_Parameters_t *params)
{
        ANN_t *ann = ANN_New(1, 1, 3, 0, 0, &ANN_Sigmoid, 1.0, 0.01);
        PSOANN_GenoToPheno(ann, params);

        float error_sum = 0.0;
        float inputs = 0.0;
        float outputs = 0.0;
        float period = 2 * PI;
        float n_samples = 0; 
        
        ANN_ResetNeuronState(ann);
        for (float i = 0; i < 1; i = i + 0.05) {
                inputs = i;
                ANN_Execute(ann, &inputs, &outputs);
                outputs = 2.0 * (outputs - 0.5);
                error_sum += fabs(sin(inputs * period) - outputs);
                n_samples++;
        }
        ANN_Delete(&ann);
        return (1 - error_sum/n_samples);
}

/* The genetic algorithms goal is set to get a float with value 100 (by mutation and reproduction) */
void controller_init() 
{
        ANN_t *ann = ANN_New(1, 1, 3, 0, 0, &ANN_Sigmoid, 2000.0, 0.01);
        int n_pop = 30;
        PSO_Particle_t *particles[n_pop];
        PSO_SwarmParams_t swarm;
        PSO_Particle_t *best = NULL; 

        for (int i = 0; i < n_pop; i++) {
                ANN_RandomizeWeights(ann);
                //ANN_Print(ann);
                particles[i] = PSO_NewParticle(PSOANN_PhenoToGeno(ann));
        }
        
        swarm.inertia = 0.1;
        swarm.acceleration_local  = 0.06;
        swarm.acceleration_global = 0.01;
        swarm.acceleration_random = 0.01;
        swarm.nr_informants = n_pop / 3;
        swarm.size = n_pop;

        unsigned int cnt = 0;
        do {
                best = PSO(&swarm, particles, PSOANN_Fitness);
                if (cnt % 10000 == 0)
                        ase_printf("FITNESS: %3.6f\n", best->fitness);
                cnt++;
        } while (best->fitness < 0.9)

        ase_printf("Done!\n");



        /* Cleanup */
        for (int i = 0; i < n_pop; i++) {
                PSO_DeleteParticle(&particles[i]);
        }

        ANN_Delete(&ann);
        exit(0);
}
