#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/control/controller.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>

static int baud = 7;

int main(int argc, char *argv[])
{
	dynamixelApi_setup(baud, ase_handleMessage);
	ase_init(controller_init);
	while(1) {
		ase_act();
	}
	printf("Controller terminated\n");
	return 0;
}

