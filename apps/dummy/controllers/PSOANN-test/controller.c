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
#include <ase/control/strategies/PSO/pso.h>

#define PI 3.14159265

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

}

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
        float error_sum = 0.0;
        float inputs = 0.0;
        float outputs = 0.0;
        float period = 2 * PI;
        float n_samples = 0; 
 
        ANN_t *ann = ANN_New(1, 1, 5, 0, 0, &ANN_Sigmoid, 1.0, 0.01);
        PSOANN_GenoToPheno(ann, params);
       
        ANN_ResetNeuronState(ann);
        for (float i = 0; i < 1; i = i + 0.05) {
                inputs = i;
                ANN_Execute(ann, &inputs, &outputs);
                outputs = 2.0 * (outputs - 0.5);
                error_sum += fabs(sin(inputs * period) - outputs);
                n_samples++;
        }
        ANN_Delete(&ann);
        //printf("FEVAL: %3.6f\n", (1 - error_sum/n_samples));
        return (1 - error_sum/n_samples);
}

/* The genetic algorithms goal is set to get a float with value 100 (by mutation and reproduction) */
void controller_init() 
{
        ANN_t *ann = ANN_New(1, 1, 5, 0, 0, &ANN_Sigmoid, 200.0, 0.01);
        int n_pop = 30;
        PSO_Particle_t *particles[n_pop];
        PSO_SwarmParams_t swarm;
        PSO_Particle_t *best = NULL; 

        for (int i = 0; i < n_pop; i++) {
                ANN_RandomizeWeights(ann);
                //ANN_Print(ann);
                particles[i] = PSO_NewParticle(PSOANN_PhenoToGeno(ann));
        }
        best = particles[0];

        swarm.inertia = 0.8;
        swarm.acceleration_local  = 0.01;
        swarm.acceleration_global = 0.01;
        swarm.acceleration_random = 0.01;
        swarm.nr_informants = n_pop / 3;
        swarm.size = n_pop;

        unsigned int cnt = 0;
        do {
                PSO(&swarm, particles, PSOANN_Fitness, 100.0, -100.0, &best);
                if (cnt % 1000 == 0) {
                        ase_printf("BEST FITNESS: %3.6f BEST: %3.6f\n", best->best_fitness, PSO_GetFittestOfAll(&swarm, particles, PSOANN_Fitness)->best_fitness);
                        for (int i = 0; i < n_pop; i++) {
                                ase_printf("[%i]: %3.6f %3.6f %i\n", i, particles[i]->fitness, particles[i]->best_fitness, particles[i]->no_fit_change_cnt);
                        }
                        ase_printf("\n\n\n");
                }
                //if (cnt == 10) exit(0);
                cnt++;
        } while (best->fitness < 0.9);

        ase_printf("Done!\n");

        /* Cleanup */
        for (int i = 0; i < n_pop; i++) {
                PSO_DeleteParticle(&particles[i]);
        }

        ANN_Delete(&ann);
        exit(0);
}
