#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/MTRANApi.h>
#include <ase/tools/IDContainer/IDContainer.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/CPGManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/BroadCaster.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/SnakeShiftHead.h>
#include <ase/control/behaviors/atron/locomotion/SnakeGait/SnakeGait.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/communication/StateSharing/StateSharing.h>
#include <ase/communication/Gossip/GossipManager.h>

#define N_MODULES	8

int internalState = 0;
int oldState=0;
gradient_struct* IDGradient;
timer_struct* timer;
void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel);
void Timer_Fired(int id);

void controller_init() {
	printf("Running\n");
	srand (getRandomSeed());
	MCManager_init();
	IDContainer_init();
	broadcaster_init();
	SnakeGait_init();
	DSManager_init(-1);
	TimerManager_init();
	GossipManager_init();
	StateSharing_init();
	timer = TimerManager_createPeriodicTimer(250, 0, Timer_Fired);
	IDGradient = GradientManager_createGradient(1, 2);
	int i;
	for(i=1;i<=5;i++)	MCManager_installHandler(i, CommandHandler);
}

void Timer_Fired(int id) {
	//atronApi_setLeds(~atronApi_getLeds());
}

void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
  printf("Got callback, payload length = %i \n", length);
}

int setIDFromGradient() {
	if(IDContainer_getSoftID()==-1) {
		int val = gradient_getValue(IDGradient);
		if(val!=0) {
			IDContainer_setSoftID(val);
			printf("I got ID = %i\n", IDContainer_getSoftID());
			//atronApi_setLeds(val);
			return 1;
		}
		return 0;
	}
	return 1;
}


void controller_act()  {
	if(oldState!=getState()) {
		internalState = 0;
	}
	oldState = getState();
	if(internalState==0) {
	  SnakeShiftHead_init(mtranApi_getHardwareID());
	  internalState++;
	}
	else if(internalState==1) {
	  if(!SnakeShiftHead_isDone()) {
	    SnakeShiftHead_act();
	  }
	  //	  else {
	  //  internalState = 0;
	  //}
	}
	DSManager_act();
	mtranApi_yield();
	if(broadcaster_act()>0) { }
	if(MCManager_act()>0) { }
	if(TimerManager_act()>0) { }
	if(GossipManager_act()>0) { }
}

void handleMessage(char* message, char messageSize, char channel) {
	/*if(channel!=8) {
		if((rand()%100)>50) {
			return;
		}
	}*/
	//atronApi_setLeds(~atronApi_getLeds()); //makes ussr crach

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
