#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/SPSALearning/SPSALearning.h>

SPSALearning_t SPSAProcess;
int nParam = 4;
void controller_init() {
	ase_printf("Hello world\n");
	SPSALearning_init(&SPSAProcess);
	SPSAProcess.ak = 0.01;
	SPSAProcess.ck = 0.01;
	SPSAProcess.nParameters = nParam;
	int n=0;
	for(n=0;n<100;n++) {
		float reward =0;
		int i;
		for(i=0;i<nParam;i++) {
			float val = SPSALearning_getTheta(&SPSAProcess, i, 0);
			ase_printf("%f, ",val);
			reward += val;
		}
		ase_printf("\n");
		ase_printf("REWARD = %f \n", reward);
		SPSALearning_collectReward(&SPSAProcess, reward);
		SPSALearning_update(&SPSAProcess);
	}
	exit(0);
//	SPSALearning_collectReward(SPSALearning_t* process, float reward);
}
