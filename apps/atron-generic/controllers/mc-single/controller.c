#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/tools/atron/AtronStatus/AtronStatus.h>
#include <ase/communication/Broadcast/Broadcaster.h>
#include <ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/tools/Timer/TimerManager.h>

#define MC_COMMAND_PAUSED				1
#define MC_COMMAND_SET_LEDS				2
#define MC_COMMAND_CONNECT				3
#define MC_COMMAND_DISCONNECT			4
#define	MC_COMMAND_ROTATE_HOME			5
#define MC_COMMAND_ROTATE				6
#define MC_COMMAND_ROTATE_CONTINUOUS	7
#define MC_COMMAND_ROTATE_TO			8
#define MC_COMMAND_ROTATE_TO_VIA		9
#define MC_COMMAND_STOP					10
#define MC_COMMAND_BRAKE				11
#define MC_COMMAND_PRINT_STATUS			12
#define MC_COMMAND_ROTATE_DEGREES		13
#define MC_COMMAND_SEND_HEART_BEAT		14
#define MC_COMMAND_ROTATE_MS			15
#define N_MC_COMMANDS					15

Timer_t* periodicTimer;
int paused = 1;
int command = 0;
uint8_t lastCommand = 255;
void sendHeatHeart();
void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
	lastCommand = mcType;
	switch(mcType) {
		case MC_COMMAND_PAUSED:
			if(length > 0) {
				paused = payload[0];
				ase_printf("Pause = %i \n", paused);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_SET_LEDS:
			if(length > 0) {
				atronApi_setLeds(payload[0]);
				ase_printf("Leds = %i \n", payload[0]);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_CONNECT:
			if(length > 0) {
				int i;
				for(i=0;i<length;i++) {
					atronApi_connect(payload[i]);
					printf("Connecting %i \n", payload[i]);
				}
			}
			else { printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_DISCONNECT:
			if(length > 0) {
				int i;
				for(i=0;i<length;i++) {
					atronApi_disconnect(payload[i]);
					ase_printf("Disconnecting %i \n", payload[i]);
				}
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_ROTATE_HOME:
			ase_printf("Homing\n");
			atronApi_home();
			break;

		case MC_COMMAND_ROTATE_CONTINUOUS:
			if(length > 0) {
				if(payload[0]==0) atronApi_rotateContinuous(-1.0f);
				if(payload[0]==1) atronApi_rotateContinuous(1.0f);
				ase_printf("Rotate Cont. %i \n", payload[0]);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_ROTATE_TO:
			if(length > 0) {
				int goalDegree = (int) (1.4118f*payload[0]);
				atronApi_rotateToDegreeInDegrees(goalDegree);
				ase_printf("Rotate to %i \n", goalDegree);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;

		case MC_COMMAND_ROTATE_TO_VIA:
			if(length > 1) {
				int goalDegree = (int) (1.4118f*payload[0]);
				int viaDegree = (int) (1.4118f*payload[1]);
				atronApi_rotateToViaInDegrees(goalDegree,viaDegree);
				ase_printf("Rotate to %i via %i\n", goalDegree, viaDegree);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;
		case MC_COMMAND_ROTATE_DEGREES:
			if(length > 1) {
				int deltaDegree = (int) (1.4118f*payload[0]);
				if(payload[1]==0) {
					deltaDegree = -deltaDegree;
				}
				atronApi_rotateDegreesX(deltaDegree);
				ase_printf("Rotate to %i \n", deltaDegree);
			}
			else { ase_printf("Length error in command = %i\n",mcType); }
			break;
		case MC_COMMAND_ROTATE_MS:
			if(length > 1) {
				int ms = (int) 100*payload[0];
				bool dir = (int) (payload[1]==0)?false:true;
				atronApi_rotateMs(ms, dir);
				ase_printf("Rotate ms %i dir %i\n", ms, dir);
			}
			break;
		case MC_COMMAND_STOP:
			ase_printf("Stopping\n");
			atronApi_centerStop();
			break;

		case MC_COMMAND_BRAKE:
			ase_printf("Breaking\n");
			atronApi_centerBrake();
			break;
		case MC_COMMAND_PRINT_STATUS:
			AtronStatus_print();
			break;
		case MC_COMMAND_SEND_HEART_BEAT:
			sendHeatHeart();
			break;
		default:
			ase_printf("Unrecognized Command = %i\n",mcType);
	}
}

char led = 1;
typedef struct {
	uint8_t typeID;
	uint8_t battery;
	uint8_t minSinceReset;
	uint8_t secSinceReset;
	uint8_t lastCommand;
	uint8_t currentPos;
	uint8_t irMsgCount;
	uint8_t xbeeMsgCount;
} HeartBeatMsg;

HeartBeatMsg msg;

int batteryCounter=0;
void sendHeatHeart() {
	msg.typeID = 99;
	batteryCounter++;
	msg.battery = batteryCounter; //battery level
	msg.minSinceReset = (getLocalMsTime()/1000)/60;
	msg.secSinceReset = (getLocalMsTime()/1000)%60;
	msg.lastCommand = lastCommand;
	msg.currentPos = atronApi_getAngularPositionDegrees();
	msg.irMsgCount = AtronStatus_getTotalIRMsgCount();
	msg.xbeeMsgCount = AtronStatus_getTotalXbeeMsgCount();
	int success = atronApi_sendMessage((char*)&msg,sizeof(HeartBeatMsg),8);
	ase_printf("Sending Xbee - success? %i\n", success);
}

void heartBeat_timer_fired(int id) {
	led = ~led;
	atronApi_setLeds(led);
	sendHeatHeart();
}
//0xff 0x1 0x2 0x22 0x6 0x0 0x0
void controller_init() {
	ase_printf("%i: Running\n",getHardwareID());
	atronApi_yield();
	broadcaster_init();
	MCManager_init();
	AtronStatus_init();
	int i;
	for(i=1;i<=N_MC_COMMANDS;i++)  {
		MCManager_installHandler(i, CommandHandler);
	}
	MCManager_setID(getHardwareID());
	atronApi_setLeds(1);
	periodicTimer = TimerManager_createPeriodicTimer(1000, 0, heartBeat_timer_fired);
	atronApi_setLeds(1);
}

void controller_act()  { }

void handleMessage(char* message, char messageSize, char channel) { }
