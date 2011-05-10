#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ANN/ann.h>

void controller_init() {
	float input[2];
        float output;
        ANN_t *n = ANN_New(2, 1, 6, 0, 0, &ANN_Sigmoid, 10.0, 0.0);

        input[0] = 0.1;
        input[1] = 0.312;
        ANN_Execute(n, input, &output);
        ANN_Print(n);
        int i;
        for(i=0;i<10;i++) {
        	ase_printf("Output: %3.2f\n", output);
        	ANN_Execute(n, input, &output);
        }
	exit(0);
}
