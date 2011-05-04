#include <stdbool.h>
#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiEventManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiTraining.h>

int LuiTraining_createKnnInput(signed char* input, char nInputs, signed char* knnInput) {
	int i;
	knnInput[0] = LuiEventManager_timeSinceEvent();
	for(i=0;i<nInputs;i++) {
		knnInput[i+1] = input[i]; 
	}
	return nInputs+1;
}


int LuiTraining_knn_train(kNN_t* kNN_process, signed char* outputs, char nOutput) {
	signed char knnInput[10];
	int nKnnInput  = LuiTraining_createKnnInput(LuiManager_getDeviceReadList(),  (char) LuiManager_getNumberOfInputDevices(), knnInput);
	int information = kNN_trainingSetInformation(kNN_process,  knnInput, nKnnInput, outputs, nOutput); 
	if(information>0) {
		if(kNN_addTraningSet(kNN_process, knnInput, nKnnInput, outputs, nOutput) == true) {
			ase_printf("kNN added traning (info = %i)\n", information);
			//ase_printf("#play learning.wav 50 1\n");
			return  1;
		}
		else {
		  	return  2;
		}
	}
	else {
		ase_printf("kNN ignored traning set with %i information \n", information);
		return  0;
	}
}
