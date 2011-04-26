#include <ase/control/strategies/SPSALearning/SPSALearning.h>
#include <ase/communication/CommDefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

SPSALearning_struct process;
void controller_init() {
	SPSALearning_init(&process);
}
static float f(float x) {
	return -(10*x-5)*(10*x-5);
}

static float f2(float x, float y, float z) {
	return -(10*x-5)*(10*x-5)-(10*y-5)*(10*y-5)-(10*z-5)*(10*z-5);
}

int multi=1;
void controller_act()  {
	if(!multi) {
		float x = SPSALearning_getTheta(&process, 0);
		float x_best = SPSALearning_getThetaNonPerturbed(&process, 0);
		float y = f(x);
		SPSALearning_collectReward(&process, y);
		SPSALearning_update(&process);
		printf("x_best=%f x=%f y=%f\n",x_best,x,y);
	}
	else {
		float x = SPSALearning_getTheta(&process, 0);
		float y = SPSALearning_getTheta(&process, 1);
		float z = SPSALearning_getTheta(&process, 2);
		float f = f2(x,y,z);
		SPSALearning_collectReward(&process, f);
		SPSALearning_update(&process);
		printf("%f at (%f, %f, %f)\n",f, x, y, z);
	}
}

int main() {
	srand(1);
	controller_init();
	int i;
	for(i=0;i<50;i++) {
		controller_act();
	}
	return 0;
}

void handleMessage(char* message, char messageSize, char channel) { }
