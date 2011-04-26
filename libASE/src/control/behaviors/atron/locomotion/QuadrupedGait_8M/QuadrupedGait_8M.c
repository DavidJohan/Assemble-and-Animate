#include <stdio.h>
#include <ase/control/behaviors/atron/locomotion/QuadrupedGait_8M/QuadrupedGait_8M.h>
#include <ase/targets/atron/AtronApi.h>

void QuadrupedGait_8M_init() {

}

void QuadrupedGait_8M_act(int id)  {
	if(id==5||id==8) {
		atronApi_rotateContinuous(1);
	}
	else if(id==4||id==1) {
		atronApi_rotateContinuous(-1);
	}
}
