#include <stdio.h>
#include <stdint.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/config/ConfigASE.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>


MCManager_command MC_command;
MCManager_handler MC_handlers[MC_MAX_HANDLERS];
uint8_t MCManager_myID = 255;



void MCManager_setID(uint8_t ID) {
	MCManager_myID = ID;
}
int MCManager_getID() {
	return MCManager_myID;
}

uint8_t MCManager_signalHandlers(MCManager_command* MC_command) {
	uint8_t i, handleCount=0;
	for(i=0;i<MC_MAX_HANDLERS;i++)  {
		if(MC_handlers[i].isUsed && MC_handlers[i].mcType==MC_command->msg.mcType) {
			MC_handlers[i].handler(MC_command->msg.mcType, MC_command->msg.payload, MC_command->payloadLength, MC_command->channel);
			handleCount++;
		}
	}
	return handleCount;
}

uint8_t MCManager_handleCommunication(MCManager_command* MC_command) {
	if(MC_command->msg.recID == MC_BROADCAST) {
		//for me and every one else send broadcast
		broadcaster_broadcast((char*) &(MC_command->msg), MC_command->payloadLength+4, MC_command->channel);
		return 1;
	}
	else if(MC_command->msg.recID == MC_WILDCARD) {
		//don't send further
		return 1;
	}
	else if(MC_command->msg.recID == MCManager_myID) {
		//for me, don't send further
		return 1;
	}
	else  {
		//not for me, therefore broadcast to find recepient - could me replaced with somthing smarter
		broadcaster_broadcast((char*) &(MC_command->msg), MC_command->payloadLength+4, MC_command->channel);
		return 0;
	}
}

static void act(char* topic, Event_t* event) {
	uint8_t handleCount=0;
	if(MC_command.isUnhandled) {
		uint8_t forMe = MCManager_handleCommunication(&MC_command);
		if(forMe) {
			handleCount += MCManager_signalHandlers(&MC_command);
		}
		MC_command.isUnhandled = 0;
	}
}


void MCManager_installHandler(uint8_t mcType, void (handler)(uint8_t, uint8_t*, uint8_t, uint8_t)) {
	uint8_t i, foundOne=0;
	for(i=0;i<MC_MAX_HANDLERS;i++)  {
		if(!MC_handlers[i].isUsed) {
			MC_handlers[i].mcType = mcType;
			MC_handlers[i].handler = handler;
			MC_handlers[i].isUsed = 1;
			foundOne = 1;
			break;
		}
	}
	if(!foundOne) {
		//ERROR: Insufficient space for number of handlers, please increase avalable space (MAX_HANDLERS)
		#ifdef PRINTF
		ase_printf("ERROR: #1 in MCManager\n");
		#endif
	}
}

static void handleMessage(Msg_t* msg) {
	if(msg->message[0]==MODULAR_COMMANDER_MESSAGE) {
		if(msg->messageSize<4) {
			//Warning: Modular Commander Message too short
			#ifdef PRINTF
			ase_printf("Warning: #1 in MCManager\n");
			#endif
			return;
		}
		if(msg->messageSize>(MC_MAX_PAYLOAD+4)) {
			//Warning: Modular Commander Message too long
			#ifdef PRINTF
			ase_printf("Warning: #2 in MCManager\n");
			#endif
			return;
		}
		MC_command.msg.msgType		= MODULAR_COMMANDER_MESSAGE;
		MC_command.msg.msgID		= (uint8_t) msg->message[1];
		MC_command.msg.recID		= (uint8_t) msg->message[2];
		MC_command.msg.mcType		= (uint8_t) msg->message[3];
		MC_command.payloadLength = (uint8_t) (msg->messageSize - 4);
		MC_command.channel = (uint8_t) msg->channel;
		uint8_t i;
		for(i=0;i<MC_command.payloadLength;i++) {
			MC_command.msg.payload[i] = (uint8_t) msg->message[i+4];
		}
		MC_command.isUnhandled = 1;
	}
}

void MCManager_init() {
	MCManager_myID = 255;
	MsgManager_subscribe(MODULAR_COMMANDER_MESSAGE, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
}
