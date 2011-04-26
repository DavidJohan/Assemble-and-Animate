#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <math.h>
#include <stdio.h>


typedef struct {
	char message[BROADCASTER_MAX_MSG_SIZE];
	char messageSize;
	char channel;
	char scheduleForSend;
	char nResends;
} broadcaster_msg;

static broadcaster_msg cacheMsgs[BROADCASTER_MSG_CACHE_SIZE];
static int broadcaster_index = 0;

static int broadcaster_sendMsgToNeighborsExcept(int cacheIndex);
static void act(char* topic, Event_t* event) {
	int i, count=0;
	for(i=0;i<BROADCASTER_MSG_CACHE_SIZE;i++) {
		if(cacheMsgs[i].scheduleForSend==1) {
			int wasSend = broadcaster_sendMsgToNeighborsExcept(i);
			if(wasSend!=0) {
				cacheMsgs[i].scheduleForSend = 0;
				cacheMsgs[i].nResends = 0;
				count+=wasSend;
			}
			else  {
				#ifdef PRINTF
				cacheMsgs[i].nResends++;
				ase_printf("Warning: Broadcaster will resend (for the %i time)...\n",cacheMsgs[i].nResends);
				#endif
			}
		}
	}
	//return count;
}

void broadcaster_init() {
	int i;
	broadcaster_index = 0;
	for(i=0;i<BROADCASTER_MSG_CACHE_SIZE;i++) {
		cacheMsgs[i].messageSize=0;
		cacheMsgs[i].channel=0;
		cacheMsgs[i].scheduleForSend=0;
		cacheMsgs[i].nResends = 0;
	}
	EventManager_subscribe(ACT_EVENT, act);
}

static broadcaster_msg* broadcaster_getSpace() {
	broadcaster_msg* cacheMsg = &cacheMsgs[broadcaster_index];
	broadcaster_index = (broadcaster_index+1)%BROADCASTER_MSG_CACHE_SIZE;
	return cacheMsg;
}

static broadcaster_msg* broadcaster_cacheMsg(char* message, char messageSize, char channel) {
	broadcaster_msg* cacheMsg = broadcaster_getSpace();
	cacheMsg->messageSize = messageSize;
	cacheMsg->channel = channel;
	int i;
	for(i=0;i<messageSize;i++) {
		cacheMsg->message[i]=message[i];
	}
	cacheMsg->message[i]=0;
	return cacheMsg;
}

static int broadcaster_checkCache(char* message, char messageSize, char channel) {
	int i,j;
	for(i=0;i<BROADCASTER_MSG_CACHE_SIZE;i++) {
		if(cacheMsgs[i].messageSize==messageSize) {
			int count = 0;
			for(j=0;j<messageSize;j++) {
				if(cacheMsgs[i].message[j]==message[j]) {
					count++;
				}
				else {
					j=messageSize;
				}
			}
			if(count==messageSize) {
				return 1;
			}
		}
	}
	return 0;
}

static int broadcaster_sendMsgToNeighborsExcept(int cacheIndex) {
	return sendMessageToAllExcept(cacheMsgs[cacheIndex].message, cacheMsgs[cacheIndex].messageSize, cacheMsgs[cacheIndex].channel);
}



int broadcaster_broadcast(char* message, char messageSize, char channel) {
	int seen = broadcaster_checkCache(message, messageSize, channel);
	if(!seen) {
		broadcaster_msg* msg = broadcaster_cacheMsg(message, messageSize, channel);
		msg->scheduleForSend = 1;
		return 0;
	}
	else {
		return 1;
	}
}
