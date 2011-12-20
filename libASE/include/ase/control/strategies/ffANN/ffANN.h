#ifndef FFANN_H
#define FFANN_H
#define ffANN_LEARNING_TYPE_SPSA					0
#define ffANN_LEARNING_TYPE_BACK_PROPAGATION		1

typedef struct {
		unsigned char learning_type;
        unsigned int nr_inputs;
        unsigned int nr_outputs;
        unsigned int nr_hidden;
        float learning_rate_hidden;
        float learning_rate_output;
        float** wi2h;
        float** wh2o;
        float* theta_hidden;
        float* theta_output;
        float* hidden;
} ffANN_t;

ffANN_t* ffANN_New(
				unsigned int nr_inputs,
                unsigned int nr_outputs,
                unsigned int nr_hidden,
                float learning_rate_hidden,
                float learning_rate_output
				);

void ffANN_Execute(ffANN_t *ann, float *inputs, float *output);
float ffANN_Train(ffANN_t *ann, float *inputs, float *tOutput);
float ffANN_TrainFullSet(ffANN_t *ann, int nTrainSets, float tInputs[nTrainSets][ann->nr_inputs], float tOutput[nTrainSets][ann->nr_outputs]);
void ffANN_setLearningType(ffANN_t *ann, int type);
void ffANN_Randomize(ffANN_t *ann);
void ffANN_print(ffANN_t *ann);

#endif
