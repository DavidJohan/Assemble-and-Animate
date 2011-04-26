#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <ase/ase/targets/atron/AtronApi.h>
#include <ase/ase/communication/CommDefs.h>
#include <ase/ase/tools/IDContainer/IDContainer.h>
#include <ase/ase/communication/RemoteControl/ModularCommander/MCManager.h>
#include <ase/ase/infrastructure/MsgManager/MsgManager.h>
#include <ase/ase/targets/AbstractModuleApi.h>
#include <ase/ase/communication/Broadcast/Broadcaster.h>
#include <ase/ase/control/strategies/DistributedStateMachine/DistributedStateManager.h>
#include <ase/ase/tools/Timer/TimerManager.h>
#include <ase/ase/communication/StateSharing/StateSharing.h>
#include <ase/ase/control/behaviors/generic/IDManager_MC/IDManager_MC.h>
#include <ase/ase/communication/Gossip/GossipManager.h>
#include <ase/ase/communication/SharedTimer/SharedTimer.h>
#include <ase/ase/ModuleStats.h>
#include <ase/ase/infrastructure/MemManager/MemManager.h>


int internalState = 0;
int oldState=0;
int currentGait=0;
long gaitTimeOffset = 0;
long lastGaitUpdateTime;
void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel);
void Gait_Timer_Fired(int id);

unsigned char ittCount=0;

void controller_init() {
	ase_printf("%i: Running \n",getHardwareID());
	srand (getRandomSeed());
	MCManager_init();
	GossipManager_init();
	IDContainer_init();
	broadcaster_init();
	TimerManager_init();
	StateSharing_init();
	SharedTimer_init(250);
	IDManager_MC_init();
	int i;
	for(i=1;i<=5;i++)	MCManager_installHandler(i, CommandHandler);
	atronApi_setLeds(255);

	setState(1);
	setPaused(0);

	if(getHardwareID()==4)  IDContainer_setSoftID(0);
	else if(getHardwareID()==65) IDContainer_setSoftID(1);
	else if(getHardwareID()==7)  IDContainer_setSoftID(2);
	else if(getHardwareID()==81)  IDContainer_setSoftID(3);
	else if(getHardwareID()==26)  IDContainer_setSoftID(4);
	else {
		atronApi_setLeds(255-15);
		IDContainer_setSoftID(getHardwareID());
	}
	gaitTimeOffset = SharedTimer_getSharedMsTime();
	lastGaitUpdateTime = SharedTimer_getSharedMsTime();
}


void CommandHandler(uint8_t mcType, uint8_t* payload, uint8_t length, uint8_t channel) {
	//printf("Got callback, payload length = %i \n", length);
	if(mcType==1) { //control if paused
		setPaused(payload[0]);
		ase_printf("Pause = %i \n", isPaused());
	}
	else if(mcType==2) { //set command
		if(getState()!=payload[0]) {
			setState(payload[0]);
			currentGait = payload[1];
		}
		ase_printf("Current command set = %i \n", getState());
	}
	if(channel==8) { // message from serial interface or zigbee is always the "newest"
		setTimeStamp(getTimeStamp()+1);
	}
}


#define ROT_DELAY 9000
long g0Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g0[8][4] = { //turn left on spot
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 180, 270}
};
long g1Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g1[8][4] = { //turn right on spot
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 270, 180, 90}
};
long g2Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g2[8][4] = { //forward
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 90, 0, 90},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};
long g3Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g3[8][4] = { //backward
	{0, 270, 180, 90},
	{0, 270, 180, 90},
	{0, 90, 180, 270},
	{0, 90, 180, 270},
	{0, 90, 0, 90},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};

long g4Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g4[8][4] = { //stop
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};

long g5Time[4] = {ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4, ROT_DELAY/4};
int g5[8][4] = { //new forward
	{0, 90, 180, 270},
	{180, 270, 0, 90},
	{0, 270, 180, 90},
	{180, 90,0, 270},
	{0, 90, 0, 90},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
	{0, 0, 0, 0},
};

int selectGait() {
	if(SharedTimer_getSharedMsTime()< gaitTimeOffset+2*(long)ROT_DELAY) {
		return 4;
	}
	else if(SharedTimer_getSharedMsTime()<gaitTimeOffset+4*(long)ROT_DELAY) {
		return 1;
	}
	else if(SharedTimer_getSharedMsTime()<gaitTimeOffset+6*(long)ROT_DELAY) {
		return 2;
	}
	else if(SharedTimer_getSharedMsTime()<gaitTimeOffset+8*(long)ROT_DELAY) {
		return 4;
	}
	else if(SharedTimer_getSharedMsTime()<gaitTimeOffset+10*(long)ROT_DELAY) {
		return 3;
	}
	else if(SharedTimer_getSharedMsTime()<gaitTimeOffset+12*(long)ROT_DELAY) {
		return 5;
	}
	else {
		gaitTimeOffset = SharedTimer_getSharedMsTime();
		return 4;
	}
}

int gaitT = 0;
int getAngle(int gait) {
	int id = IDContainer_getSoftID(), angle=0;
	long delay=0;
	if(gait==0) delay = g0Time[gaitT];
	if(gait==1) delay = g1Time[gaitT];
	if(gait==2) delay = g2Time[gaitT];
	if(gait==3) delay = g3Time[gaitT];
	if(gait==4) delay = g4Time[gaitT];
	if(gait==5) delay = g5Time[gaitT];

	long cTime = SharedTimer_getSharedMsTime();
	if((lastGaitUpdateTime+delay)<cTime) {
		gaitT = (gaitT+1)%4;
		lastGaitUpdateTime += delay;
		if(lastGaitUpdateTime<cTime) {
			lastGaitUpdateTime = cTime;
		}
	}
	if(gait==0) angle = g0[id][gaitT];
	if(gait==1) angle = g1[id][gaitT];
	if(gait==2) angle = g2[id][gaitT];
	if(gait==3) angle = g3[id][gaitT];
	if(gait==4) angle = g4[id][gaitT];
	if(gait==5) angle = g5[id][gaitT];
	return angle;
}
int counter =0;
void controller_act()  {
	if(oldState!=getState()) {
		internalState = 0;
	}
	oldState = getState();
	//if(getHardwareID()==0) printf("%li %li %li\n",SharedTimer_getSharedMsTime(), getLocalMsTime(),SharedTimer_getSharedMsTime()-getLocalMsTime());
	//if(counter%10==0) printf("{%i, %li, %li, %li},\n",getHardwareID(),time(NULL),getLocalMsTime(),SharedTimer_getSharedMsTime());
	counter++;
	if(isPaused()) { //pause module
		if(internalState==0) {
			atronApi_centerStop();
			internalState=1;
		}
	}
	else if(getState() == 1) { //free running robot
		int gait = selectGait();
		int angle = getAngle(gait);
		atronApi_rotateToDegreeInDegrees(angle);
	}
	else if(getState() == 2) { //remote controlled gait
		int angle = getAngle(currentGait);
		atronApi_rotateToDegreeInDegrees(angle);
	}
	else if(getState() == 3) { //home module
		if(internalState==0) {
			atronApi_rotateToDegree(0);
			internalState=1;
		}
	}
	if(broadcaster_act()>0) { }
	if(MCManager_act()>0) { }
	if(TimerManager_act()>0) { }
	if(GossipManager_act()>0) { }

	if(ittCount%500==0) {
		ModuleStats_print();
	}
	ittCount++;
}

void handleMessage(char* message, char messageSize, char channel) {
	ModuleStats_gotMessage(message, messageSize, channel);
	int nSubscribers = MsgManager_handleMessage(message, messageSize, channel);
	if(nSubscribers==0) {
		ase_printf("Warning: Unrecognized message type received %i \n",message[0]);
	}
}
