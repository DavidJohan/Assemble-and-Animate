#include <stdio.h>
#include <stdlib.h>

#include <ase/infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/communication/Message.h>
#include <ase/tools/Timer/TimerManager.h>

#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/controller.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>
#include "CM510Behaviors.h"
#include "controller.h"
#include "BeatDetector.h"
static kNN_t* kNN_behavior;
static kNN_t* kNN_compound;
static Playback_t* playback_data;

static CM510Behavior_dance_t dance_data;
static CM510Behavior_monkey_song_t monkey_data;
static CM510Behavior_bird_song_t bird_data;
static CM510Behavior_seek_t seek_data;
static CM510Behavior_escape_t escape_data;
static CM510Behavior_play_dead_t play_dead_data;
static CM510Behavior_attack_t attack_data;
static CM510Behavior_play_t play_data;
static CM510Behavior_hide_t hide_data;
static CM510Behavior_follow_t follow_data;


void installBehaviors(Subsumption_t* subsumption) {
	LuiBehaviorManager_addBehavior(1, &escape_data, escape_start, escape_act, escape_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(2, &seek_data, seek_start, seek_act, seek_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(3, &play_data, play_start, play_act, play_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(4, &bird_data, bird_song_start, bird_song_act, bird_song_stop,  's', subsumption);
	LuiBehaviorManager_addBehavior(5, &monkey_data, monkey_song_start, monkey_song_act, monkey_song_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(6, &attack_data, attack_start, attack_act, attack_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(7, &dance_data, dance_start, dance_act, dance_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(104, &play_dead_data, play_dead_start, play_dead_act, play_dead_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(103, &hide_data, hide_start, hide_act, hide_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(8, &follow_data, follow_start, follow_act, follow_stop,'s', subsumption);

	LuiBehaviorManager_addBehavior(97, kNN_compound, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 'c', subsumption);
	LuiBehaviorManager_addBehavior(98, playback_data, playback_start, playback_act, playback_stop, 'r', subsumption);
	LuiBehaviorManager_addBehavior(99, kNN_behavior, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 't', subsumption);
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

/*dynaCtrl_t dynaCtrl[2] = {
		{0, 0, -1023, 1023, 1023},
		{0, 0, -1023, 1023, 1023}};*/

dynaCtrl_t dynaCtrl[4] = {
		{0, 0, -1023, 1023, 1023},
		{0, 0, -1023, 1023, 1023},
		{512, 0, 512+100, 1023-100, 500},
		{512, 0, 512-200, 512+200, 500}
};


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
		if(dynamixelApi_isWheelMode(i)) {
			int speed = (int)(10.23f*outputValues[i]);//;(int)((outputValues[i]<50)?2*10.23f*(outputValues[i]) : 2*10.23f*(outputValues[i]-50)+1024);
			//ase_printf("%i: speed = %i (%i)\n", i, speed, outputValues[i]);
			if(speed<0) speed = abs(speed) + 1024;
			dynamixelApi_setMovingSpeed(i, speed);
		}
		else {
			float percentage = ((float)outputValues[i]+100)/200.0f; //assumed [-100,100]
			dynaCtrl[i].cPos = (int)(percentage*(dynaCtrl[i].maxPos-dynaCtrl[i].minPos)+dynaCtrl[i].minPos);
			if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
			if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
			dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);
		}
		//ase_printf("%i ", dynaCtrl[i].cPos);
	}
	//ase_printf("\n");
}

int getInputType(int index) {
	if(index==0) return DISTANCE;
	else if(index==1) return DISTANCE;
	else if(index==2) return DISTANCE;
	else if(index==3) return LONG_DISTANCE;//NOISE;
	else return NONE;
}

int getOutputType(int index) {
	if(dynamixelApi_isWheelMode(index)) return WHEEL;
	else return JOINT;
}
static signed char crop(int x, int min, int max) {
	if(x>max) return max;
	else if(x<min) return min;
	else return x;
}
int getControlInput(signed char* inputValues, char maxInputs, bool* readSuccess){
	inputValues[0] = crop(dynamixelApi_CM510_getADC(4)/4, 0, 100); //top dist
	inputValues[1] = crop(dynamixelApi_CM510_getADC(5)/4, 0, 100); //left dist
	inputValues[2] = crop(dynamixelApi_CM510_getADC(0)/4, 0, 100); //right dist
	inputValues[3] = crop((dynamixelApi_CM510_getADC(3)-50)/5, 0, 100); //right dist
	//crop(100*(dynamixelApi_CM510_getMicEventCount()%2), 0, 100); //mic
	//adc 2 = extra brick
	//adc 1 =
	//adc 3 =
	//ase_printf("%i %i %i \n", dynamixelApi_CM510_getADC(1), dynamixelApi_CM510_getADC(2), dynamixelApi_CM510_getADC(3));
	ase_printf("Dist = %i\n", inputValues[3]);
	*readSuccess = true;
	return 0;
}

int getNumberOfInputs() {
	return 4;
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
		dynamixelApi_setWheelMode(i, true);
		dynamixelApi_setCompliance(i,1,254);
	}
}

void carArmInit() {
	dynamixelApi_setup(1,NULL); delay_ms(250);
	dynamixelApi_connect(11); delay_ms(50);
	dynamixelApi_connect(12); delay_ms(50);

	dynamixelApi_connect(4); delay_ms(50);
	dynamixelApi_connect(14); delay_ms(50);
	int i;
	for(i=0;i<dynamixelApi_countActuators();i++) {
		if(i<2) {
			dynamixelApi_setWheelMode(i, true);
			dynamixelApi_setCompliance(i,1,254);
		}
		else {
			dynamixelApi_setWheelMode(i, false);
			dynamixelApi_setCompliance(i,1,254);
		}
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

int brickID_old=-1;
void checkForBehaviorBrickTimer(int id) {
	int values[20];
	for(int i=0;i<20;i++) {
		values[i] = dynamixelApi_CM510_getADC(2);
	}
	qsort (values, 20, sizeof(int), intcompare);
	int brickID, val;
	val = values[10]; //median ADC value
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
	if(brickID!=brickID_old) {
		//ase_printf("Median ADC value = %i, brickID = %i\n", val, brickID);
		configureLUIBoard(brickID);
		brickID_old = brickID;
	}

	signed char sensorVal[5]; bool success;
	getControlInput(sensorVal, 5, &success);
	//ase_printf("%i %i %i \n", sensorVal[0],sensorVal[1],sensorVal[2]);
	if(sensorVal[0]	>5||sensorVal[1]>5||sensorVal[2]>5) {
		for(int i=1;i<=6;i++) {
			dynamixelApi_CM510_setLed(i);
		}
	}
	else {
		for(int i=1;i<=6;i++) {
			dynamixelApi_CM510_clearLed(i);
		}
	}
}

void aliveTimer(int id) {
	ase_printf("alive...\n");
}

void controller_init() {
  dynamixelApi_CM510_init();
  //ase_printf("*************************\n" );
  //ase_printf("*Dynamixel-CM510 Started*\n" );
  //ase_printf("************************\n" );
  //flowerInit();
  //carInit();
  srand(10);
  carArmInit();
  LUI_init();
  BeatDetector_init();
  TimerManager_createPeriodicTimer(250, 0, checkForBehaviorBrickTimer); //reads behavior brick directly
  TimerManager_createPeriodicTimer(1000, 0, aliveTimer); //reads behavior brick directly

  ase_printf("size of knn %i\n",sizeof(kNN_t));
  ase_printf("size of playback %i\n",sizeof(Playback_t));

  kNN_behavior = (kNN_t*) MemManager_malloc(sizeof(kNN_t));
  kNN_compound = (kNN_t*) MemManager_malloc(sizeof(kNN_t));
  playback_data = (Playback_t*) MemManager_malloc(sizeof(Playback_t));
  if(kNN_behavior == NULL || kNN_compound == NULL || playback_data == NULL) {
	  ase_printf("Out of memory! \n");
  }
  kNN_init(kNN_behavior, 1);
  kNN_setMode(kNN_behavior, KNN_MODE_MEAN);
  kNN_init(kNN_compound, 1);
  kNN_setMode(kNN_compound, KNN_MODE_MOST_FREQUENT_SET);
  Playback_init(playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
  configureLUIBoard(0);


  //ase_printf("Init done\n");
}
