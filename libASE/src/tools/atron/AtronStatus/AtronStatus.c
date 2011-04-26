#include <ase/tools/atron/AtronStatus/AtronStatus.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/tools/IDContainer/IDContainer.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <stdio.h>

static unsigned int msgCount[9];
static unsigned int conState[8];

static void updateState() {
	int i;
	for(i=0;i<8;i++) {
		conState[i] = atronApi_isConnected(i);
	}
}

void AtronStatus_print() {
	updateState();
	ase_printf("Stats for module [%i, %i] \n",getHardwareID(), IDContainer_getSoftID());
	ase_printf(" Msg North [%i,%i,%i,%i]\n",msgCount[0],msgCount[1],msgCount[2],msgCount[3]);
	ase_printf(" Msg South [%i,%i,%i,%i]\n",msgCount[4],msgCount[5],msgCount[6],msgCount[7]);
	ase_printf(" Msg XBee  [%i]\n",msgCount[8]);

	ase_printf(" Con North [%i,%i,%i,%i]\n",conState[0],conState[1],conState[2],conState[3]);
	ase_printf(" Con South [%i,%i,%i,%i]\n",conState[4],conState[5],conState[6],conState[7]);

	ase_printf(" Battery Level [%i]\n", atronApi_getBatteryLevel());
	ase_printf(" Center [angle, rot?] = [%i, %i]\n\n",atronApi_getAngularPositionDegrees(),atronApi_isRotating());

}

int AtronStatus_getTotalIRMsgCount() {
	int i, sum =0;
	for(i=0;i<8;i++) {
		sum = msgCount[i];
	}
	return sum;
}
int AtronStatus_getTotalXbeeMsgCount() {
	return msgCount[8];
}
void gotMessage(char* topic, Event_t* event) {
	Msg_t* msg = (Msg_t*) event->val_prt;
	int channel = (int) msg->channel;
	if(channel<9) {
		msgCount[channel]++;
	}
}

void AtronStatus_init() {
	EventManager_subscribe(MSG_RECV_EVENT,gotMessage);
}
