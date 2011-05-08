#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/control/strategies/ANN/ann.h>

#define PI 3.14159265

SPSALearning_t spsa_process;
int n_params = 16;

float SPSANN_Fitness(ANN_t *ann)
{
        float error_sum = 0.0;
        float inputs = 0.0;
        float outputs = 0.0;
        float period = 2 * PI;
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

void controller_init() {
	ase_printf("SPSANN test\n");
	SPSALearning_init(&spsa_process);
	spsa_process.ak = 0.01;
	spsa_process.ck = 0.01;
	spsa_process.nParameters = n_params;

        ANN_t *ann = ANN_New(1, 1, 2, 0, 0, &ANN_Tanh, 200.0, 0.01);

	for (int n = 0; n < 100000; n++) {
		float reward = 0;
		float theta = 0;
                int x = 0, y = 0;
                for (int i = 0; i < n_params; i++) {
                        if (i > 0 && i % (int)sqrt(n_params) == 0)
                                x++;
                        y = i % (int)sqrt(n_params);
			theta = SPSALearning_getTheta(&spsa_process, i, 0);
                        //ase_printf("X: %i Y: %i\n", x, y);
			ann->weights[x][y] += theta;
                        reward += SPSANN_Fitness(ann);
		}

		if (n % 100 == 0)
                        ase_printf("REWARD = %f \n", reward);
		SPSALearning_collectReward(&spsa_process, reward);
		SPSALearning_update(&spsa_process);
	}
	exit(0);
}
