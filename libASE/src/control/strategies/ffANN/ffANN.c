#include <ase/control/strategies/ffANN/ffANN.h>

#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <float.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/MemManager/MemManager.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>

ffANN_t* ffANN_New(	unsigned int nr_inputs,
					unsigned int nr_outputs,
					unsigned int nr_hidden,
					float learning_rate_hidden,
					float learning_rate_output) {

	ffANN_t* ann = (ffANN_t*) MemManager_Xmalloc(sizeof(ffANN_t));
	ann->learning_type = ffANN_LEARNING_TYPE_BACK_PROPAGATION;
	ann->nr_inputs = nr_inputs;
	ann->nr_hidden = nr_hidden;
	ann->nr_outputs = nr_outputs;
	ann->learning_rate_hidden = learning_rate_hidden;
	ann->learning_rate_output= learning_rate_output;

	ann->wi2h = (float **) MemManager_Xmalloc(sizeof(float *) * nr_inputs);
	for (int i = 0; i < nr_inputs; i++) {
			ann->wi2h[i] = (float *) MemManager_Xmalloc(sizeof(float) * nr_hidden);
	}

	ann->wh2o = (float **) MemManager_Xmalloc(sizeof(float *) * nr_hidden);
	for (int i = 0; i < nr_hidden; i++) {
			ann->wh2o[i] = (float *) MemManager_Xmalloc(sizeof(float) * nr_outputs);
	}
	ann->hidden  = (float *) MemManager_Xmalloc(sizeof(float) * nr_hidden);
	ann->theta_hidden  = (float *) MemManager_Xmalloc(sizeof(float) * nr_hidden);
	ann->theta_output  = (float *) MemManager_Xmalloc(sizeof(float) * nr_outputs);

	ffANN_Randomize(ann);
	return ann;
}
void ffANN_Randomize(ffANN_t *ann) {
	for (int i = 0; i < ann->nr_inputs; i++) {
		for (int j = 0; j < ann->nr_hidden; j++) {
			ann->wi2h[i][j] = ((float)rand())/((float)RAND_MAX);
		}
	}
	for (int i = 0; i < ann->nr_hidden; i++) {
		for (int j = 0; j < ann->nr_outputs; j++) {
			ann->wh2o[i][j] = ((float)rand())/((float)RAND_MAX);
		}
	}
	for (int i = 0; i < ann->nr_hidden; i++) {
		ann->theta_hidden[i] = 1.25f*((float)rand())/((float)RAND_MAX)-0.25f;
	}
	for (int i = 0; i < ann->nr_outputs; i++) {
		ann->theta_output[i] = 1.25f*((float)rand())/((float)RAND_MAX)-0.25f;
	}
}

void ffANN_Execute(ffANN_t *ann, float *inputs, float *output) {
	for(int i=0;i<ann->nr_hidden;i++) {
		ann->hidden[i] = 0;
		for(int j=0;j<ann->nr_inputs;j++) {
			ann->hidden[i] += ann->wi2h[j][i]*inputs[j];
		}
		ann->hidden[i] = (1.0 / (1 + exp(- (ann->hidden[i] + ann->theta_hidden[i]))));
	}
	for(int i=0;i<ann->nr_outputs;i++) {
		output[i] = 0;
		for(int j=0;j<ann->nr_hidden;j++) {
			output[i] += ann->wh2o[j][i]*ann->hidden[j];
		}
		output[i] = (1.0 / (1 + exp(- (output[i] + ann->theta_output[i]))));
	}
}
float ffANN_Train(ffANN_t *ann, float *inputs, float *tOutput) {
	float output[ann->nr_outputs];
	float errorSum = 0;
	ffANN_Execute(ann, inputs, output);
	float e[ann->nr_outputs];
	for(int i=0;i<ann->nr_outputs;i++) {
		e[i] = output[i]*(1-output[i])*(tOutput[i]-output[i]);
		float dTheta = ann->learning_rate_output * e[i];
		ann->theta_output[i] += dTheta;
		for(int j=0;j<ann->nr_hidden;j++) {
			float dWeight = dTheta * ann->hidden[j];
			ann->wh2o[j][i] += dWeight;
		}
		errorSum += fabs(tOutput[i]-output[i]);
	}
	for(int i=0;i<ann->nr_hidden;i++) {
		float g = 0;
		for(int j=0;j<ann->nr_outputs;j++) {
			g += ann->wh2o[i][j]*e[j];
		}
		float eh = ann->hidden[i] * (1 - ann->hidden[i]) * g;
		float dTheta = ann->learning_rate_hidden * eh;
		ann->theta_hidden[i] += dTheta;
		for(int j=0;j<ann->nr_inputs;j++) {
			float dWeight = dTheta * inputs[j];
			ann->wi2h[j][i] += dWeight;
		}
	}
	return errorSum;
}

SPSALearning_t ANNTrainer;
static void weights2SPSA(ffANN_t *ann) {
	int index = 0;
	for(int i=0;i<ann->nr_inputs;i++) {
		for(int j=0;j<ann->nr_hidden;j++) {
			SPSALearning_setThetaAt(&ANNTrainer, index, ann->wi2h[i][j]);
			index++;
		}
	}
	for(int i=0;i<ann->nr_hidden;i++) {
		for(int j=0;j<ann->nr_outputs;j++) {
			SPSALearning_setThetaAt(&ANNTrainer, index, ann->wh2o[i][j]);
			index++;
		}
	}
	for (int i = 0; i < ann->nr_hidden; i++) {
		SPSALearning_setThetaAt(&ANNTrainer, index, ann->theta_hidden[i]);
		index++;
	}
	for (int i = 0; i < ann->nr_outputs; i++) {
		SPSALearning_setThetaAt(&ANNTrainer, index, ann->theta_output[i]);
		index++;
	}
}

static void SPSA2Weights(ffANN_t *ann) {
	int index = 0;
	for(int i=0;i<ann->nr_inputs;i++) {
		for(int j=0;j<ann->nr_hidden;j++) {
			ann->wi2h[i][j] = SPSALearning_getTheta(&ANNTrainer, index, 0);
			index++;
		}
	}
	for(int i=0;i<ann->nr_hidden;i++) {
		for(int j=0;j<ann->nr_outputs;j++) {
			ann->wh2o[i][j] = SPSALearning_getTheta(&ANNTrainer, index, 0);
			index++;
		}
	}
	for (int i = 0; i < ann->nr_hidden; i++) {
		ann->theta_hidden[i] = SPSALearning_getTheta(&ANNTrainer, index, 0);
		index++;
	}
	for (int i = 0; i < ann->nr_outputs; i++) {
		ann->theta_output[i] = SPSALearning_getTheta(&ANNTrainer, index, 0);
		index++;
	}
}

void ffANN_print(ffANN_t *ann) {
	ase_printf("******  ANN Print  *******\n");

	ase_printf("  Input to Hidden: \n");
	for(int i=0;i<ann->nr_inputs;i++) {
		for(int j=0;j<ann->nr_hidden;j++) {
			ase_printf("%f, ",ann->wi2h[i][j]);
		}
	}
	ase_printf("\n");

	ase_printf("  Hidden to Output: \n");
	for(int i=0;i<ann->nr_hidden;i++) {
		for(int j=0;j<ann->nr_outputs;j++) {
			ase_printf("%f, ",ann->wh2o[i][j]);
		}
	}
	ase_printf("\n");

	ase_printf("  Theta Hidden: \n");
	for (int i = 0; i < ann->nr_hidden; i++) {
		ase_printf("%f, ",ann->theta_hidden[i]);
	}
	ase_printf("\n");

	ase_printf("  Theta Output: \n");
	for (int i = 0; i < ann->nr_outputs; i++) {
		ase_printf("%f, ",ann->theta_output[i]);
	}
	ase_printf("\n*************************\n\n ");
}
void ffANN_setLearningType(ffANN_t *ann, int type) {
	ann->learning_type = type;
	if(type==ffANN_LEARNING_TYPE_SPSA) {
		SPSALearning_init(&ANNTrainer);
		ANNTrainer.ak = 0.1f;
		ANNTrainer.ck = 0.1f;
		ANNTrainer.nowrap = 1;
		ANNTrainer.nParameters = (ann->nr_inputs+1)*(ann->nr_hidden+1)+(ann->nr_hidden+1)*(ann->nr_outputs+1);
		weights2SPSA(ann);
		SPSA2Weights(ann);
	}
}

float ffANN_getError(ffANN_t *ann, float* tInput, float* tOutput) {
	float output[ann->nr_outputs];
	float errorSum = 0;
	ffANN_Execute(ann, tInput, output);
	for(int i=0;i<ann->nr_outputs;i++) {
		errorSum +=fabs(tOutput[i]-output[i]);
	}
	return errorSum;
}

float ffANN_TrainFullSet(ffANN_t *ann, int nTrainSets, float tInputs[nTrainSets][ann->nr_inputs], float tOutput[nTrainSets][ann->nr_outputs]) {
	float errorSum=0;
	if(ann->learning_type==ffANN_LEARNING_TYPE_SPSA) {
		for(int i=0;i<nTrainSets;i++) {
			float error0 = 0;
			SPSA2Weights(ann);
			error0 += ffANN_getError(ann, tInputs[i], tOutput[i]);
			SPSALearning_collectReward(&ANNTrainer, -error0);
			SPSALearning_update(&ANNTrainer);
			errorSum+=error0;

			float error1=0;
			SPSA2Weights(ann);
			error1 += ffANN_getError(ann, tInputs[i], tOutput[i]);
			SPSALearning_collectReward(&ANNTrainer, -error1);
			SPSALearning_update(&ANNTrainer);
			errorSum+=error1;
		}

		errorSum /=2;
	}
	else if(ann->learning_type==ffANN_LEARNING_TYPE_BACK_PROPAGATION){
		for(int i=0;i<nTrainSets;i++) {
			errorSum += ffANN_Train(ann, tInputs[i], tOutput[i]);
		}
	}
	else {
		ase_printf("error\n");
	}

	return errorSum;
}

