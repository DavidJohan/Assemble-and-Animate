#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/controller.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>
#include "CM510Behaviors.h"
//static kNN_t kNN_behavior;
//static kNN_t kNN_compound;
//static Playback_t playback_data;

//static CM510Behavior_fly_t fly_data;
static CM510Behavior_dance_t dance_data;
static CM510Behavior_bee_song_t bee_data;
static CM510Behavior_bird_song_t bird_data;
//static CM510Behavior_geiger_t geiger_data;
//static CM510Behavior_move_t move_data;
static CM510Behavior_escape_t escape_data;

void installBehaviors(Subsumption_t* subsumption) {
	LuiBehaviorManager_addBehavior(1, &escape_data, escape_start, escape_act, escape_stop,'s', subsumption);
	//LuiBehaviorManager_addBehavior(2, &move_data, move_start, move_act, move_stop, 's', subsumption);
	//LuiBehaviorManager_addBehavior(3, &geiger_data, geiger_start, geiger_act, geiger_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(4, &bird_data, bird_song_start, bird_song_act, bird_song_stop,  's', subsumption);
	LuiBehaviorManager_addBehavior(5, &bee_data, bee_song_start, bee_song_act, bee_song_stop, 's', subsumption);
	//LuiBehaviorManager_addBehavior(6, &fly_data, fly_start, fly_act, fly_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(7, &dance_data, dance_start, dance_act, dance_stop, 's', subsumption);
	//LuiBehaviorManager_addBehavior(97,&kNN_compound, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 'c', subsumption);
	//LuiBehaviorManager_addBehavior(98,&playback_data, playback_start, playback_act, playback_stop, 'r', subsumption);
	//LuiBehaviorManager_addBehavior(99,&kNN_behavior, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 't', subsumption);
}

typedef struct {
	int cPos;
	int minPos;
	int maxPos;
	int maxVel;
} dynaCtrl_t;

dynaCtrl_t dynaCtrl[6] = {
		{512, 0, 1024, 500},
		{512, 0, 1024, 500},
		{512, 0, 1024, 500},
		{512, 0, 1024, 500},
		{512, 0, 1024, 500},
		{512, 0, 1024, 500}};


void applyControlOutput(signed char* outputValues, char nOutput){
	for(int i=0;i<nOutput;i++) {
		int vel = dynaCtrl[i].maxVel*(outputValues[i]/100.0f);
		int deltaPos = vel/10; //10hz?
		ase_printf("%i -> %i, ",dynaCtrl[i].cPos, dynaCtrl[i].cPos + deltaPos);
		dynaCtrl[i].cPos +=deltaPos;
		if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
		if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
		dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);
	}
	ase_printf("\n");
}

int getControlInput(signed char* inputValues, char maxInputs, bool* readSuccess){
	inputValues[0] = dynamixelApi_CM510_getADC(0); //scaled?
	*readSuccess = true;
	return 0;
}

int getNumberOfInputs() {
	return 1;
}

int getNumberOfOutputs(){
	return dynamixelApi_countActuators();
}

void delay_ms(long delay_ms) {
	long startTime = dynamixelApi_getMsTime();
	while((startTime+delay_ms)>dynamixelApi_getMsTime());
}


void flowerInit() {
	dynamixelApi_setup(1,NULL); delay_ms(250);
	dynamixelApi_connect(8); delay_ms(50);
	dynamixelApi_connect(15); delay_ms(50);

	dynamixelApi_connect(5); delay_ms(50);
	dynamixelApi_connect(6); delay_ms(50);

	dynamixelApi_connect(9); delay_ms(50);
	dynamixelApi_connect(7); delay_ms(50);
	int i;
	for(i=0;i<dynamixelApi_countActuators();i++) {
		dynamixelApi_setWheelMode(i, false);
	}
}


void controller_init() {
  dynamixelApi_CM510_init();
  ase_printf("*************************\n" );
  ase_printf("*Dynamixel-CM510 Started*\n" );
  ase_printf("************************\n" );
  flowerInit();
  LUI_init();
  //kNN_init(&kNN_behavior, 1);
  //kNN_init(&kNN_compound, 1);
  //Playback_init(&playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
  ase_printf("Init done\n");
}
