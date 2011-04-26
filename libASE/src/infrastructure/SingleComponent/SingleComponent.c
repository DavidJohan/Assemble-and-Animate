#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/tools/IDContainer/IDContainer.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/CentralPatternGenerator.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/CPGManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/communication/StateSharing/StateSharing.h>
#include <ase/communication/Gossip/GossipManager.h>

int internalState = 0;
int oldState=0;
gradient_t* IDGradient;
Timer_t* timer;
static void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel);
void Timer_Fired(int id);

// Pointers to behavior
static void (*component_init)(int);
static int (*component_act)();
static bool (*component_done)();

// Component parameters
int initialization_parameter;

void SingleComponent_init(int init_param, void (*init_fn)(int), int (*act_fn)(), bool (*done_fn)()) {
	ase_printf("SingleComponent running\n");
	srand (getRandomSeed());
	initialization_parameter = init_param;
	component_init = init_fn;
	component_act = act_fn;
	component_done = done_fn;
	MCManager_init();
	GradientManager_init();
	CPGManager_init();
	IDContainer_init();
	broadcaster_init();
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

static void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
  ase_printf("Got callback, payload length = %i \n", length);
}

void SingleComponent_act()  {
	if(oldState!=getState()) {
		internalState = 0;
	}
	oldState = getState();
	if(internalState==0) {
	  (*component_init)(initialization_parameter);
	  internalState++;
	}
	else if(internalState==1) {
	  if(!(*component_done)()) {
	    (*component_act)();
	  }
	}
	DSManager_act();
	if(broadcaster_act()>0) { }
	if(MCManager_act()>0) { }
	if(TimerManager_act()>0) { }
	if(GossipManager_act()>0) { }
}

void SingleComponent_handleMessage(char* message, char messageSize, char channel) {
	if(MsgManager_handleMessage(message, messageSize, channel)==0) {
		ase_printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
