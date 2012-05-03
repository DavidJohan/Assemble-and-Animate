#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/control/controller.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/targets/dynamixel.h>
#include <ase/targets/dynamixel/dynamixel-cm510/firmware/serial.h>

static unsigned char buffer[50];
static int bufferIndex = 0;
void handleSerialMessages() {
	while(serial_get_qstate()>0) {
		serial_read(&buffer[bufferIndex], 1);
		//ase_printf("%c", buffer[bufferIndex]); //ecco
		if(buffer[bufferIndex]=='\n'||buffer[bufferIndex]=='\r') {
			buffer[bufferIndex+1] = 0;
		//	ase_printf("Echo Line: %s \n", buffer);
			ase_handleMessage((char*)buffer, bufferIndex, 0);
			bufferIndex = 0;
		}
		else {
			bufferIndex++;
		}
	}
}
int main(int argc, char *argv[])
{
	//dynamixelApi_setup(baud, ase_handleMessage);
	ase_init(controller_init);
	while(1) {
		ase_act();
		handleSerialMessages();
	}
	ase_printf("Controller terminated\n");
	return 0;
}

