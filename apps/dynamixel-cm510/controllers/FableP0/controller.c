#include <stdio.h>
#include <stdlib.h>

#include <ase/infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/communication/Message.h>
#include <ase/tools/Timer/TimerManager.h>

#include <ase/control/strategies/kNN/kNN.h>
//#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/controller.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>
#include "BeatDetector.h"
#include "FableBehaviors.h"
#include "SnakeBehavior.h"
#include "HumanoidBehavior.h"
#include "controller.h"

//#define WALKER
//#define SNAKE
#define HUMANOID

//static Playback_t* playback_data;
//static CM510Behavior_osscilate_t osscilate_data;
//static CM510Behavior_logPos_t logPos_data;

#ifdef WALKER
static CM510Behavior_cpg_t cpg_data;
#endif

#ifdef SNAKE
static CM510Behavior_snake_t snake_data;
#endif

#ifdef HUMANOID
static CM510Behavior_humanoid_t humanoid_data;
#endif


void installBehaviors(Subsumption_t* subsumption) {
	#ifdef WALKER
	//LuiBehaviorManager_addBehavior(1, &osscilate_data, osscilate_start, osscilate_act, osscilate_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(1, &cpg_data, cpg_start, cpg_act, cpg_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(2, &logPos_data, logPos_start, logPos_act, logPos_stop,'s', subsumption);
	//LuiBehaviorManager_addBehavior(98, playback_data, playback_start, playback_act, playback_stop, 'r', subsumption);
	#endif

	#ifdef SNAKE
	LuiBehaviorManager_addBehavior(1, &snake_data, snake_start, snake_act, snake_stop,'s', subsumption);
	#endif

	#ifdef HUMANOID
	LuiBehaviorManager_addBehavior(1, &humanoid_data, humanoid_start, humanoid_act, humanoid_stop,'s', subsumption);
	#endif

}

typedef struct {
	int cPos;
	float cVel;
	int minPos;
	int maxPos;
	int maxVel;
} dynaCtrl_t;

#ifndef HUMANOID
dynaCtrl_t dynaCtrl[8] = {
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023}
};
#endif

#ifdef HUMANOID
dynaCtrl_t dynaCtrl[6] = {
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
		{512, 0, 512-300, 512+300, 1023},
};
#endif




void applyControlOutput(signed char* outputValues, char nOutput){
	for(int i=0;i<nOutput;i++) {
		if(outputValues[i]!=127) { //do nothing
			if(dynamixelApi_isWheelMode(i)) {
				int speed = (int)(10.23f*outputValues[i]);//;(int)((outputValues[i]<50)?2*10.23f*(outputValues[i]) : 2*10.23f*(outputValues[i]-50)+1024);
				if(speed<0) speed = abs(speed) + 1024;
				dynamixelApi_setMovingSpeed(i, speed);
			}
			else {
				float percentage = ((float)outputValues[i]+100)/200.0f; //assumed [-100,100]
				dynaCtrl[i].cPos = (int)(percentage*(dynaCtrl[i].maxPos-dynaCtrl[i].minPos)+dynaCtrl[i].minPos);
				if(dynaCtrl[i].cPos>dynaCtrl[i].maxPos) dynaCtrl[i].cPos = dynaCtrl[i].maxPos;
				if(dynaCtrl[i].cPos<dynaCtrl[i].minPos) dynaCtrl[i].cPos = dynaCtrl[i].minPos;
				dynamixelApi_setGoalPos(i, dynaCtrl[i].cPos);
				//ase_printf("Dyna (%i) pos = %i \n", i, dynaCtrl[i].cPos);
			}
		}
	}
}

int getInputType(int index) {

	#ifdef HUMANOID
	if(index==0) return LONG_DISTANCE;
	#endif
	return NONE;
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
	#ifdef HUMANOID
	inputValues[0] = crop((dynamixelApi_CM510_getADC(3)-50)/5, 0, 100); //right dist
	#endif
	*readSuccess = true;
	return 0;
}

int getNumberOfInputs() {
	#ifdef WALKER
	return 0;
	#endif

	#ifdef SNAKE
	return 0;
	#endif

	#ifdef HUMANOID
	return 1;
	#endif
}

int getNumberOfOutputs(){
	return dynamixelApi_countActuators();
}

void delay_ms(long delay_ms) {
	long startTime = dynamixelApi_getMsTime();
	while((startTime+delay_ms)>dynamixelApi_getMsTime());
}

void fableModuleInit() {
	delay_ms(250);
	dynamixelApi_setup(1, NULL); delay_ms(250);

	#ifdef WALKER
	dynamixelApi_connect(LEFT_FRONT_UPPER_ID); delay_ms(150);
	dynamixelApi_connect(LEFT_FRONT_LOWER_ID); delay_ms(150);
	dynamixelApi_connect(RIGHT_FRONT_UPPER_ID); delay_ms(150);
	dynamixelApi_connect(RIGHT_FRONT_LOWER_ID); delay_ms(150);
	dynamixelApi_connect(LEFT_BACK_UPPER_ID); delay_ms(150);
	dynamixelApi_connect(LEFT_BACK_LOWER_ID); delay_ms(150);
	dynamixelApi_connect(RIGHT_BACK_UPPER_ID); delay_ms(150);
	dynamixelApi_connect(RIGHT_BACK_LOWER_ID); delay_ms(150);
	#endif

	#ifdef SNAKE
	dynamixelApi_connect(1); delay_ms(150);
	dynamixelApi_connect(16); delay_ms(150);
	dynamixelApi_connect(7); delay_ms(150);
	dynamixelApi_connect(9); delay_ms(150);
	dynamixelApi_connect(8); delay_ms(150);
	dynamixelApi_connect(15); delay_ms(150);
	dynamixelApi_connect(6); delay_ms(150);
	dynamixelApi_connect(5); delay_ms(150);
	#endif

	#ifdef HUMANOID
	dynamixelApi_connect(1); delay_ms(150);
	dynamixelApi_connect(16); delay_ms(150);
	dynamixelApi_connect(6); delay_ms(150);
	dynamixelApi_connect(5); delay_ms(150);

	for(int i=0;i<getNumberOfActuators();i++) {
		dynamixelApi_setCompliance(i,1,254);
	}

	/*dynamixelApi_connect(7); delay_ms(150);
	dynamixelApi_connect(9); delay_ms(150);
	dynamixelApi_connect(8); delay_ms(150);
	dynamixelApi_connect(15); delay_ms(150);*/

	#endif
}

void configureLUIBoard(int behaviorID) {
	char msg[11] = {LUI_SETUP,
					behaviorID, 0, 0, 0, 0,
					1, 0, 0, 0, 0};
	ase_handleMessage(msg, 11, 0);
}

int intcompare (const void * a, const void * b) {
  return ( *(int*)a - *(int*)b );
}

void aliveTimer(int id) {
	ase_printf("alive...\n");
}

void controller_init() {
  dynamixelApi_CM510_init();
  //delay_ms(5000);
  srand(10);
  fableModuleInit();
  LUI_init();
  TimerManager_createPeriodicTimer(1000, 0, aliveTimer); //reads behavior brick directly
  //playback_data = (Playback_t*) MemManager_malloc(sizeof(Playback_t));
  //if(playback_data == NULL) { ase_printf("Out of memory! \n");  }
  //Playback_init(playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
  configureLUIBoard(1);
  //dynamixelApi_CM510_setZigBee(true);
  #ifdef WALKER
  cpg_start(&cpg_data);
  #endif
  #ifdef SNAKE
  snake_start(&snake_data);
  #endif
  #ifdef HUMANOID
  BeatDetector_init();
  humanoid_start(&humanoid_data);
  #endif
}
