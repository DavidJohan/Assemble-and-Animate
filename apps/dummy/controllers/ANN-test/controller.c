#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/ANN/ann.h>

void controller_init() {
	ANN_t *n = ANN_New(2, 1, 6, 0, 0, &ANN_Sigmoid, 10.0, 0.0);
	ANN_Print(n);
	exit(0);
}
