#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/control/strategies/ANN/ann.h>
#include <ase/control/strategies/GA/ga.h>

#define PI 3.14159265
static SPSALearning_t spsa_process;
static int n_params = 7*7;

float SPSANN_Fitness(ANN_t * ann, bool printOutput)
{
	int n_samples = 0;
	float error_sum = 0.0;
	float inputs = 1; //only bias as input
	float outputs = 0.0;
	float period =  2*PI;
	ANN_ResetNeuronState(ann);
	for (float i = 0; i < 1; i = i + 0.05) {
			ANN_Execute(ann, &inputs, &outputs);
			outputs = 2.0f*(outputs - 0.5f);
			float target = sin(i * period);
			error_sum += fabs(target - outputs);
			n_samples++;
			if(printOutput) ase_printf("{%f, %f},", target, outputs);
	}
	if(printOutput) ase_printf("\n");
	return 1-error_sum/n_samples;
}


void controller_init() {
	ase_printf("SPSANN test\n");
	SPSALearning_init(&spsa_process);
	spsa_process.ck = 0.01f;
	spsa_process.ak = 0.01f;
	spsa_process.nowrap = 1;
	spsa_process.nParameters = n_params;
    ANN_t *ann = ANN_New(1, 1, 5, 0, 0, &ANN_Sigmoid, 0.02, 0.01);
    for (int  i = 0; i < n_params; i++) {
    	SPSALearning_setThetaAt(&spsa_process, i, 0.0f); //initialize all weights to 0
    }
    SPSALearning_reset(&spsa_process,1);
    for (int n = 0; n < 500000; n++) {
		//SPSALearning_decayAkCk(&spsa_process, 1.0f, 1.0f, 0, 0.602f, 0.101f);//0.602f, 0.101f
		float reward = 0;
		float theta = 0;
		int x = 0, y = 0;
		for (int i = 0; i < n_params; i++) {
				if (i > 0 && i % (int)sqrt(n_params) == 0) x++;
				y = i % (int)sqrt(n_params);
				theta = SPSALearning_getTheta(&spsa_process, i, 0);
				ann->weights[x][y] = theta;
		}
		reward = SPSANN_Fitness(ann, false);
		if(n%10000 == 0) {
			ase_printf("{%i, %f},\n", n,  reward);
		}

		SPSALearning_collectReward(&spsa_process, reward);
		SPSALearning_update(&spsa_process);
	}
    ANN_Print(ann);
    SPSANN_Fitness(ann, true);
	exit(0);
}
