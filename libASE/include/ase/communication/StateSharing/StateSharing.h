#include <ase/communication/Gossip/GossipManager.h>

typedef struct {
	uint8_t timeStamp;
	uint8_t paused : 1;
	uint8_t state : 7;
} state_t;

void StateSharing_init();
int StateSharing_getTimeStamp();
void StateSharing_setTimeStamp(uint8_t timeStamp);
int StateSharing_getState();
void StateSharing_setState(uint8_t state);
void StateSharing_setPaused(uint8_t paused);
int StateSharing_isPaused();
