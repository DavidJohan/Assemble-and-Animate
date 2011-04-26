#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/control/controller.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/targets/dummy.h>

int main(int argc, char *argv[])
{
	AbstractDummyApi_init();
	ase_init(controller_init);
	while(1) {
		ase_act();
	}
	printf("Controller terminated\n");
	return 0;
}

