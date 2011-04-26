#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/CPGManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/BroadCaster.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/SnakeToQuadruped.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/communication/Gossip/GossipManager.h>

void controller_init() {
	printf("Running\n");
	srand (getRandomSeed());
	MCManager_init();
	broadcaster_init();
	DSManager_init(-1);
	TimerManager_init();
	GossipManager_init();
	SnakeToQuadruped_init(1);

	atronApi_setLeds(255);
}

void controller_act()  {
	if(!SnakeToQuadruped_isDone()) {
		SnakeToQuadruped_act();
	}
	if(DSManager_act()>0) { }
	if(broadcaster_act()>0) { }
	if(MCManager_act()>0) { }
	if(TimerManager_act()>0) { }
	if(GossipManager_act()>0) { }
}

void handleMessage(char* message, char messageSize, char channel) {
	if(message[0]==MODULAR_COMMANDER_MESSAGE) {
		MCManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==CPG_MESSAGE) {
		CPGManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==GRADIENT_MESSAGE) {
		GradientManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==DISTRIBUTED_STATE_MESSAGE) {
		DSManager_handleMessage(message, messageSize, channel);
	}
	else if(message[0]==GOSSIP_MESSAGE) {
		GossipManager_handleMessage(message, messageSize, channel);
	}
	else {
		printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
