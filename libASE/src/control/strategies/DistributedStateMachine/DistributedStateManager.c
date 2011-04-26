#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateMerger.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>


uint8_t globalState = 0; /* the numerically highest state we have ever seen so far, and that we keep sending */
uint8_t localState = 255; /* the latest global state relevant to this module */
uint16_t recipientId = 0; /* the module meant to execute the state = globalState */
uint8_t pendingStates1[DSM_MAX_PENDING_STATES], pendingStates2[DSM_MAX_PENDING_STATES]; /* pending states buffers */
uint8_t *pendingStates = pendingStates1; /* actual pending states */
uint8_t *pendingBuffer = pendingStates2; /* will be swapped */
int myID;

DSManager_msg DSM_msg;

DSManager_msg DSM_rec_msg;
DSManager_update_t DSM_update;

int DSM_semaphor = 0;
int DSM_gotNewMsg = 0;

bool started;
bool alternateSequenceFlag;
bool startingModule;



void DSManager_start() {
	started = true;
    /*e2cpkt = (EightToCarMsg*)(call Packet.getPayload(&pkt, sizeof(EightToCarMsg)));
    call InfraredPacket.setConnector(&pkt, 0xFF);
    call DesyncTimer.startOneShot(DESYNC_TIMER_OFFSET + (call Random.rand16() & 0x7F));
    */
}
void DSManager_stop() {
	started = false;
}


/* send a certain state for a certain recipient */
void DSManager_sendState(uint8_t state, uint8_t recpId) {
	/* this check is necessary to avoid sending old states during rollbacks */
    if(state > globalState) {
		globalState = state;
		recipientId = recpId;
	}
}


/* add a state to the pending states vector */
void DSManager_addPendingState(uint8_t state){
	uint8_t i;
    for(i=0; i<DSM_MAX_PENDING_STATES; i++) {
		if(pendingStates[i] == state)
			return;
		if(pendingStates[i] == 0) {
			pendingStates[i] = state;
			break;
		}
	}
	#ifdef PRINTF
	ase_printf("%i: Added pending state %i\n", myID, state);
	#endif
}

/* remove a state from the pending states vector */
void DSManager_removePendingState(uint8_t state){
	uint8_t i;
    for(i=0; i< DSM_MAX_PENDING_STATES; i++) {
		if(pendingStates[i] == state) {
			pendingStates[i] = 0;
			break;
		}
	}
	#ifdef PRINTF
	ase_printf("%i: Removed pending state %i\n", myID, state);
	#endif
}

/* check whether there is any currently pending state */
bool DSManager_pendingStatesPresent() {
	uint8_t i;
    for(i=0; i<DSM_MAX_PENDING_STATES; i++) {
		if(pendingStates[i] != 0) {
			return true;
		}
    }
	return false;
}

uint8_t DSManager_getMyNewState() {
	uint8_t state = localState;
	localState = 255;
	return state;
}
uint8_t DSManager_getGlobalState() {
	return globalState;
}
void DSManager_sendStateNow() {
	uint8_t i;
    for(i=0; i<DSM_MAX_PENDING_STATES; i++) {
      DSM_msg.pendingStates[i] = pendingStates[i];
    }
	DSM_msg.msgType = DISTRIBUTED_STATE_MESSAGE;
    DSM_msg.state = globalState;
    DSM_msg.recipientId= recipientId;
    DSM_msg.alternateSequenceFlag = alternateSequenceFlag;
	sendMessageToAllNeighbors((char*)&DSM_msg, sizeof(DSM_msg));
}

void DSM_updateState() {
	DSManager_msg* msgptr = &DSM_rec_msg;
	uint8_t previousState;
	uint8_t *tmp, antitx_buf[DSM_MAX_PENDING_STATES], i;

	/* this should not apply to the module starting the sequence*/
	if(alternateSequenceFlag != msgptr->alternateSequenceFlag && (!startingModule)) {
		/* reset the vars to their init values */
		globalState = 0;
		localState = 255;
		recipientId = 0;
		for(i=0; i<DSM_MAX_PENDING_STATES; i++) pendingStates1[i] = 0;
		for(i=0; i<DSM_MAX_PENDING_STATES; i++) pendingStates2[i] = 0;
		alternateSequenceFlag = msgptr->alternateSequenceFlag;
	}
	/* if we are the starting module we should disregard msgs coming from the previous run!*/
	else if(alternateSequenceFlag != msgptr->alternateSequenceFlag && (startingModule)) {
		return;
	}

	previousState = globalState;

	for(i=0; i<DSM_MAX_PENDING_STATES; i++) {
		antitx_buf[i] = msgptr->pendingStates[i];
	}

	globalState =  DSM_merge(globalState, pendingStates, msgptr->state, antitx_buf, pendingBuffer);
	tmp = pendingStates; pendingStates = pendingBuffer; pendingBuffer = tmp; /* swap */

	if(globalState>previousState) {
		recipientId = msgptr->recipientId;
		if(msgptr->recipientId == myID )
			localState = globalState;
	}
}

int DSManager_act() {
	if(DSM_gotNewMsg) {
		DSM_semaphor = 1;
		DSM_updateState();
		DSM_gotNewMsg = 0;
		DSM_semaphor = 0;
	}
	if(DSM_update.nextUpdateTime<=getLocalTime()) { //send state every 200ms
		DSManager_sendStateNow();
		DSM_update.nextUpdateTime += DSM_update.deltaTime; //+some random
		if(DSM_update.nextUpdateTime<=getLocalTime()) { //already now - reset time?
			DSM_update.nextUpdateTime = getLocalTime() + DSM_update.deltaTime;
		}
		return 1;
	}
	return 0;
}

void DSManager_reset_state() {
	uint8_t i;
    startingModule = false;
    /* reset the vars to their init values */
    globalState = 0;
    localState = 255;
    recipientId = 0;
    for(i=0; i<DSM_MAX_PENDING_STATES; i++) pendingStates1[i] = 0;
    for(i=0; i<DSM_MAX_PENDING_STATES; i++) pendingStates2[i] = 0;
    DSM_update.deltaTime = 0.2f;
	DSM_update.nextUpdateTime = getLocalTime() + DSM_update.deltaTime;
}

void DSManager_reset() {
  DSManager_reset_state();
}

void DSManager_reset_sequence() {
  DSManager_reset_state();
  startingModule = true;
	/* flip the flag (can't do that with ~alternateSequenceFlag, beware!) */
    if(alternateSequenceFlag)
      alternateSequenceFlag = false;
    else
      alternateSequenceFlag = true;
}

static void handleMessage(Msg_t* msg) {
	if(msg->message[0]==DISTRIBUTED_STATE_MESSAGE) {
		if(DSM_semaphor==1) return;
		DSM_gotNewMsg = 0;
		int i;
		for(i=0;i<msg->messageSize;i++) {
			((char*)&DSM_rec_msg)[i] = msg->message[i];
		}
		DSM_gotNewMsg = 1;
	}
}

void DSManager_init(int id) {
	myID = id;
	DSManager_reset_state();
	alternateSequenceFlag = false;
	MsgManager_subscribe(DISTRIBUTED_STATE_MESSAGE, handleMessage);
}


