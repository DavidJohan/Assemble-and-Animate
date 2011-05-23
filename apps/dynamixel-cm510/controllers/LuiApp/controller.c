#include <stdio.h>
#include <stdlib.h>
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
	float cVel;
	int minPos;
	int maxPos;
	int maxVel;
} dynaCtrl_t;

/*dynaCtrl_t dynaCtrl[6] = {
		{512, 0, 0, 850, 500},
		{512, 0, 512, 850, 500},
		{512, 0, 0, 850, 500},
		{512, 0, 512, 850, 500},
		{512, 0, 0, 850, 500},
		{512, 0, 512, 850, 500}};*/

dynaCtrl_t dynaCtrl[2] = {
		{512, 0, 0, 1023, 1023},
		{512, 0, 0, 1023, 1023}};


void applyControlOutput(signed char* outputValues, char nOutput){
	for(int i=0;i<nOutput;i++) {
		/*dynaCtrl[i].cVel += 0.5f*(outputValues[i]-dynaCtrl[i].cVel);

		if(dynaCtrl[i].cVel>dynaCtrl[i].maxVel) dynaCtrl[i].cVel = dynaCtrl[i].maxVel;
		if(dynaCtrl[i].cVel<-dynaCtrl[i].maxVel) dynaCtrl[i].cVel = -dynaCtrl[i].maxVel;

		int deltaPos = dynaCtrl[i].cVel; //10hz?
		ase_printf("%i -> %i, ",dynaCtrl[i].cPos, dynaCtrl[i].cPos + deltaPos);
		dynaCtrl[i].cPos += deltaPos;
		if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
		if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
		dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);*/

		//direct control
		/*dynaCtrl[i].cPos = (int)(10.23f*outputValues[i]); //assumed [0,100]
		if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
		if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
		dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);
		ase_printf("%i ", dynaCtrl[i].cPos);*/

		//indirect control within a range
		float percentage = (float)outputValues[i]/100.0f; //assumed [0,100]
		dynaCtrl[i].cPos = (int)(percentage*(dynaCtrl[i].maxPos-dynaCtrl[i].minPos)+dynaCtrl[i].minPos);

		if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
		if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
		dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);
		//ase_printf("%i ", dynaCtrl[i].cPos);
	}
	//ase_printf("\n");
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
		dynamixelApi_setCompliance(i,1,254);
	}
}

void carInit() {
	dynamixelApi_setup(1,NULL); delay_ms(250);
	dynamixelApi_connect(11); delay_ms(50);
	dynamixelApi_connect(12); delay_ms(50);
	int i;
	for(i=0;i<dynamixelApi_countActuators();i++) {
		dynamixelApi_setWheelMode(i, false);
		dynamixelApi_setCompliance(i,1,254);
	}
}

void configureLUIBoard(int behaviorID) {
	char msg[11] = {LUI_SETUP,
					behaviorID, 0, 0, 0, 0,
					1, 0, 0, 0, 0};
	ase_handleMessage(msg, 11, 0);
}

int intcompare (const void * a, const void * b)
{
  return ( *(int*)a - *(int*)b );
}

void checkForBehaviorBrickTimer(int id) {
	int values[20];
	for(int i=0;i<20;i++) {
		values[i] = dynamixelApi_CM510_getADC(2);
	}
	qsort (values, 20, sizeof(int), intcompare);
	/*for(int i=0;i<20;i++) {
		ase_printf("%i ", values[i]);
	}
	ase_printf("\n");
	*/

	int brickID, val;
	val = values[10];
	if(val> 40 && val< 43) brickID = 5;
	else if(val> 95 && val< 97) brickID = 7;
	else if(val> 127 && val< 130) brickID = 102;
	else if(val> 177 && val< 180) brickID = 99;
	else if(val> 187 && val< 190) brickID = 104;
	else if(val> 211 && val< 215) brickID = 2;
	else if(val> 247 && val< 252) brickID = 6;
	else if(val> 291 && val< 296) brickID = 97;
	else if(val> 313 && val< 319) brickID = 3;
	else if(val> 347 && val< 349) brickID = 4;
	else if(val> 402 && val< 409) brickID = 103;
	else if(val> 477 && val < 485) brickID = 1;
	else if(val> 507&& val< 515) brickID = 98;
	else brickID = 0;
	ase_printf("Median ADC value = %i, brickID = %i\n", val, brickID);
	configureLUIBoard(brickID);
}

void controller_init() {
  dynamixelApi_CM510_init();
  ase_printf("*************************\n" );
  ase_printf("*Dynamixel-CM510 Started*\n" );
  ase_printf("************************\n" );
  //flowerInit();
  carInit();
  LUI_init();
  TimerManager_createPeriodicTimer(250, 0, checkForBehaviorBrickTimer);

  //kNN_init(&kNN_behavior, 1);
  //kNN_init(&kNN_compound, 1);
  //Playback_init(&playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
  configureLUIBoard(0);
  ase_printf("Init done\n");
}
