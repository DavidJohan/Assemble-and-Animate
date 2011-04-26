#include <stdio.h>
#include <ase/communication/StateSharing/StateSharing.h>
#include <ase/communication/Gossip/GossipManager.h>


gossip_struct* gossip;
state_struct myState;

void StateSharing_handleGossip(char* message, char messageSize, char channel) {
	if(messageSize!=sizeof(state_struct)) {
		printf("Warning msg has wrong size \n");
		return;
	}
	state_struct* gossipState = (state_struct*) message;
	
	if(gossipState->timeStamp>myState.timeStamp) {
		printf("Got news: %i %i %i \n", gossipState->timeStamp, gossipState->paused, gossipState->state);
		myState.timeStamp = gossipState->timeStamp;
		myState.paused = gossipState->paused;
		myState.state = gossipState->state;
		GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
	}
	//else printf("Got old news %i %i %i \n", gossipState->timeStamp, gossipState->paused, gossipState->state);
		
}

void StateSharing_init() {
	myState.timeStamp=0;
	myState.paused=1;
	myState.state=0;
	gossip = GossipManager_createGossip(1, 250, 100, StateSharing_handleGossip);
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
	GossipManager_start(gossip);
	
}
int getTimeStamp() {
	return myState.timeStamp;
}
void setTimeStamp(uint8_t timeStamp) {
	myState.timeStamp = timeStamp;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

int getState() {
	return myState.state;
}

void setState(uint8_t state) {
	myState.state = state;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

void setPaused(uint8_t paused) {
	myState.paused = paused;
	GossipManager_updateGossip(gossip, (char*)&myState, sizeof(myState));
}

int isPaused() {
	return myState.paused;
}
