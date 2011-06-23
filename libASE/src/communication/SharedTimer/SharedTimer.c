#include <stdio.h>
#include <ase/communication/SharedTimer/SharedTimer.h>
#include <ase/communication/Gossip/GossipManager.h>
#include <ase/targets/AbstractModuleApi.h>

//TODO rewrite SharedTimer to follow new ASE conventions

#ifdef USSR
 #define TYPICAL_COMM_DELAY 75
#else
 #define TYPICAL_COMM_DELAY 0
#endif

static gossip_t* gossip;
static sharedTime_t sharedTime;
static long lastUpdate;

static void updateTime() {
	long now = getLocalMsTime();
	sharedTime.timeMs += (now - lastUpdate);
	lastUpdate = now;
}

static void handleGossip(char* message, char messageSize, char channel) {
	if(messageSize!=sizeof(sharedTime)) {
		#ifdef PRINTF
		ase_printf("Warning SharedTimer msg has wrong size \n");
		#endif
		return;
	}
	sharedTime_t* neighborTime = (sharedTime_t*) message;
	updateTime();
	sharedTime.timeMs += ((neighborTime->timeMs+TYPICAL_COMM_DELAY)-sharedTime.timeMs)/2;
	GossipManager_updateGossip(gossip, (char*)&sharedTime, sizeof(sharedTime));
}

static void updateGossip(gossip_t* gossip) {
	updateTime();
	GossipManager_updateGossip(gossip, (char*)&sharedTime, sizeof(sharedTime));
}

long SharedTimer_getSharedMsTime() {
	updateTime();
	return sharedTime.timeMs;
}

float SharedTimer_getSharedTime() {
	updateTime();
	return (float)(sharedTime.timeMs/1000.0f);
}
void SharedTimer_reset() {
	sharedTime.timeMs = 0;
	lastUpdate = 0;
}

void SharedTimer_init(long msDelay) {
	gossip = GossipManager_createGossip(2, msDelay, 0*msDelay/10, handleGossip, updateGossip);
	SharedTimer_reset();
	updateTime();
	GossipManager_updateGossip(gossip, (char*)&sharedTime, sizeof(sharedTime));
	GossipManager_start(gossip);
}
