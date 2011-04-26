#include <stdio.h>
#include <stdint.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/targets/AbstractModuleApi.h>

void controller_init() {
	ase_printf("Running [ ID = %i ]\n",getHardwareID());
}

uint8_t counter = 0;
long nextUpdateTime = 0;
void controller_act()  {
	if(atronApi_getMsTime()>nextUpdateTime) {
		atronApi_setLeds(counter++);
		nextUpdateTime = atronApi_getMsTime()+10;
	}
}

void handleMessage(char* message, char messageSize, char channel) {

}
