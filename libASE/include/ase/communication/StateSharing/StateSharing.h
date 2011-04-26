#include <ase/communication/Gossip/GossipManager.h>

typedef struct {
	uint8_t timeStamp;
	uint8_t paused : 1;
	uint8_t state : 7;
} state_t;

void StateSharing_init();
int getTimeStamp();
void setTimeStamp(uint8_t timeStamp);
int getState();
void setState(uint8_t state);
void setPaused(uint8_t paused);
int isPaused();
