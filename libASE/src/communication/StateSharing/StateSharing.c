#include <stdio.h>
#include <ase/communication/StateSharing/StateSharing.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/Gossip/GossipManager.h>

//TODO generalise and clean up code

static gossip_t* gossip;
static state_t myState;


void StateSharing_handleGossip(char* message, char messageSize, char channel) {
	if(messageSize!=sizeof(state_t)) {
		#ifdef PRINTF
		ase_printf("Warning msg has wrong size \n");
		#endif
		return;
	}
	state_t* gossipState = (state_t*) message;

	if(gossipState->timeStamp>myState.timeStamp) {
		#ifdef PRINTF
		ase_printf("Got news: %i %i %i \n", gossipState->timeStamp, gossipState->paused, gossipState->state);
		#endif
		myState.timeStamp = gossipState->timeStamp;
		myState.paused = gossipState->paused;
		myState.state = gossipState->state;
		GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
	}
	//else printf("Got old news %i %i %i \n", gossipState->timeStamp, gossipState->paused, gossipState->state);

}

void StateSharing_updateGossip(gossip_t* gossip) {
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}


void StateSharing_init() {
	myState.timeStamp=0;
	myState.paused=1;
	myState.state=0;
	gossip = GossipManager_createGossip(1, 250, 100, StateSharing_handleGossip, StateSharing_updateGossip);
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
	GossipManager_start(gossip);

}
int StateSharing_getTimeStamp() {
	return myState.timeStamp;
}
void StateSharing_setTimeStamp(uint8_t timeStamp) {
	myState.timeStamp = timeStamp;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

int StateSharing_getState() {
	return myState.state;
}

void StateSharing_setState(uint8_t state) {
	myState.state = state;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

void StateSharing_setPaused(uint8_t paused) {
	myState.paused = paused;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

int StateSharing_isPaused() {
	return myState.paused;
}
