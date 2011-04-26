#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/atron/AtronStatus/AtronStatus.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/communication/CommDefs.h>

#define N_MC_COMMANDS 					3


#define MC_COMMAND_STOP					1
#define MC_COMMAND_ROTATE_CONTINUOUS	2
#define MC_COMMAND_SETID				3


void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
	int id = MCManager_getID();
	int i;
	ase_printf("%i: Got a command!!! %i \n", id, mcType);
	switch(mcType) {
		case MC_COMMAND_STOP:
			ase_printf("%i: Stopping\n",id);
			atronApi_centerStop();
			break;
		case MC_COMMAND_ROTATE_CONTINUOUS:
			if(length > 0) {
				if(payload[0]==0) atronApi_rotateContinuous(-1.0f);
				if(payload[0]==1) atronApi_rotateContinuous(1.0f);
				ase_printf("%i: Rotate Cont. %i \n", id, payload[0]);
			}
			else { ase_printf("%i: Length error in command = %i\n", id, mcType); }
			break;
		case MC_COMMAND_SETID:
			for(i=0;i<length;i++) {
				if(payload[i]==getHardwareID()) {
					MCManager_setID(i);
				}
			}
			break;
		default:
			ase_printf("%i: Unrecognized Command = %i\n",id, mcType);
	}
}

void controller_init() {
	ase_printf("%i: Running\n",getHardwareID());
	atronApi_yield();
	broadcaster_init();
	MCManager_init();
	int i;
	for(i=1;i<=N_MC_COMMANDS;i++)  {
		MCManager_installHandler(i, CommandHandler);
	}
	MCManager_setID(getHardwareID());
	atronApi_setLeds(1);
}
void controller_act()  { }

void handleMessage(char* message, char messageSize, char channel) { }
