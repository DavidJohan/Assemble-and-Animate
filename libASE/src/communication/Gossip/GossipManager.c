#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ase/communication/Gossip/GossipManager.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure.h>
#include <ase/config/ConfigASE.h>
#include <ase/communication/CommDefs.h>

//TODO rewrite Gossip manager to follow new ASE conventions
gossip_t GM_gossip[GOSSIP_MAX];
static int fired;
static void handleMessage(Msg_t* msg) {
	if(msg->message[0]==GOSSIP_MESSAGE) {
		if((msg->messageSize-2)>GOSSIP_MAX_MSG_SIZE) {
			#ifdef PRINTF
			ase_printf("Error: #3 in GossipManager\n"); //gossip message to large increase GOSSIP_MAX_MSG_SIZE
			#endif
			return;
		}
		gossip_msg* gossipMsg = ((gossip_msg*) msg->message);
		uint8_t i,j;
		for(i=0;i<GOSSIP_MAX;i++)  {
			if(GM_gossip[i].isUsed  && GM_gossip[i].label == gossipMsg->label) {
				GM_gossip[i].heard.isNew = 1;
				GM_gossip[i].heard.msgSize = msg->messageSize;
				GM_gossip[i].heard.msgChannel = msg->channel;
				for(j=0;j<msg->messageSize;j++) {
					((char*)&GM_gossip[i].heard.msg)[j] = msg->message[j];
				}
			}
		}
	}
}

static void act(char* topic, Event_t* event) {
	for(int i=0;i<GOSSIP_MAX;i++)  {
		if(GM_gossip[i].isUsed && GM_gossip[i].heard.isNew) {
			GM_gossip[i].handler(GM_gossip[i].heard.msg.payload, GM_gossip[i].heard.msgSize-2, GM_gossip[i].heard.msgChannel);
			GM_gossip[i].heard.isNew = 0;
		}
	}
}

void GossipManager_init() {
	uint8_t i;
	for(i=0;i<GOSSIP_MAX;i++)  {
		GM_gossip[i].isUsed = 0;
	}
	fired = 0;
	MsgManager_subscribe(GOSSIP_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT,act);
}

static long getDelay(long msDelay, long msVariance) {
	if(msVariance<=0) {
		return msDelay;
	}
	else {
		long randomComponent = msVariance-rand()%(2*msVariance);
		return msDelay+randomComponent;
	}
}

void GossipManager_timerFired(int index) {
	if(GM_gossip[index].isUsed) {
		GM_gossip[index].msg.msgType = GOSSIP_MESSAGE;
		GM_gossip[index].msg.label = GM_gossip[index].label;
		GM_gossip[index].updater(&GM_gossip[index]);
		sendMessageToAllNeighbors((char*)&GM_gossip[index].msg, GM_gossip[index].messageSize+2);
		long newDelay =  getDelay(GM_gossip[index].msDelay, GM_gossip[index].msVariance);
		TimerManager_setDelay(GM_gossip[index].timer, newDelay);
		fired++;
	}
}

gossip_t* GossipManager_createGossip(uint8_t label, long msDelay, long msVariance, void (handler)(char*, char, char),void (*updater)(gossip_t*))  {
	uint8_t i;
	for(i=0;i<GOSSIP_MAX;i++)  {
		if(GM_gossip[i].isUsed != 1) {
			GM_gossip[i].isUsed = 1;
			GM_gossip[i].label = label;
			GM_gossip[i].handler = handler;
			GM_gossip[i].updater = updater;
			GM_gossip[i].msDelay = msDelay;
			GM_gossip[i].msVariance = msVariance;
			GM_gossip[i].timer = TimerManager_createPeriodicTimer(getDelay(msDelay, msVariance), i, GossipManager_timerFired);
			TimerManager_pauseTimer(GM_gossip[i].timer);
			return &GM_gossip[i];
		}
	}
	#ifdef PRINTF
	ase_printf("Error: #1 in GossipManager\n"); //out of gossips, increase number of GOSSIP_MAX
	#endif
	return NULL;
}

void GossipManager_updateGossip(gossip_t* gossip, char* message, char messageSize)  {
	if(messageSize<=GOSSIP_MAX_MSG_SIZE) {
		uint8_t i;
		for(i=0;i<messageSize;i++)  {
			gossip->msg.payload[i] = message[i];
		}
		gossip->messageSize = messageSize;
	}
	else {
		#ifdef PRINTF
		ase_printf("Error: #2 in GossipManager\n"); //gossip message to large increase GOSSIP_MAX_MSG_SIZE
		#endif
	}
}

int GossipManager_remove(gossip_t* gossip) {
	if(gossip->isUsed) {
		gossip->isUsed = 0;
		return 1;
	}
	return 0;
}

int GossipManager_start(gossip_t* gossip) {
	if(gossip->isUsed) {
		TimerManager_startTimer(gossip->timer);
		return 1;
	}
	return 0;
}

int GossipManager_pause(gossip_t* gossip) {
	if(gossip->isUsed) {
		TimerManager_pauseTimer(gossip->timer);
		return 1;
	}
	return 0;
}


