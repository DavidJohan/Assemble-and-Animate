#include <stdio.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/targets/non-public/lpf.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiInterface.h>

#include "LpfBehaviors.h"

static kNN_t kNN_behavior;
static kNN_t kNN_compound;
static Playback_t playback_data; 

static LpfBehavior_fly_t fly_data;
static LpfBehavior_dance_t dance_data;
static LpfBehavior_bee_song_t bee_data;
static LpfBehavior_bird_song_t bird_data;
static LpfBehavior_geiger_t geiger_data;
static LpfBehavior_move_t move_data;
static LpfBehavior_escape_t escape_data;

void installBehaviors(Subsumption_t* subsumption) {
	LuiBehaviorManager_addBehavior(1, &escape_data, escape_start, escape_act, escape_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(2, &move_data, move_start, move_act, move_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(3, &geiger_data, geiger_start, geiger_act, geiger_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(4, &bird_data, bird_song_start, bird_song_act, bird_song_stop,  's', subsumption);
	LuiBehaviorManager_addBehavior(5, &bee_data, bee_song_start, bee_song_act, bee_song_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(6, &fly_data, fly_start, fly_act, fly_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(7, &dance_data, dance_start, dance_act, dance_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(97,&kNN_compound, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 'c', subsumption);
	LuiBehaviorManager_addBehavior(98,&playback_data, playback_start, playback_act, playback_stop, 'r', subsumption);
	LuiBehaviorManager_addBehavior(99,&kNN_behavior, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 't', subsumption);
}


//skal være en call-back
void applyControlOutput(signed char* outputValues, char nOutput) {
	int mCount  = LPFApi_getNumberOfMotors();
	int rgbCount  = LPFApi_getNumberOfRGBLights();
	int dispCount  = LPFApi_getNumberOfDisplays();
	if(nOutput==(mCount+rgbCount+dispCount)) {
		int i,index=0;
		for(i=0;i<mCount;i++) {
			signed char val = outputValues[index];
			LPFApi_setMotorPower(i, val);
			index++;
		}
		for(i=0;i<rgbCount;i++) {
			signed char val = outputValues[index];
			if(val<0) val=-val;
			LPFApi_setRGBLight(i, val, val, val);
			index++;
		}
		for(i=0;i<dispCount;i++) {//what to do here?
			index++;
			//LPFApi_printOnDisplay(0, 10, 10, 'X');
		}
	}
	else {
		ase_printf("Warning: Number of devices does not match length of output array\n");
	}
}
//skal være en call-back
int getControlInput(signed char* inputValues, char maxInputs, bool* readSuccess) {
	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	*readSuccess = true;
	signed char result[3];
	int i,index=0;
	for(i=0;i<dsCount;i++) {
		bool res = LPFApi_getDetectSensor(i, result);
		inputValues[index] = (res)?result[0]:255;
		if(!res) *readSuccess = false;
		index++;
	}
	for(i=0;i<lsCount;i++) {
		bool res = LPFApi_getLightSensor(i, result);
		inputValues[index] = (res)?result[0]:255;
		if(!res) *readSuccess = false;
		index++;
	}
	for(i=0;i<tsCount;i++) {
		bool res = LPFApi_getTiltSensor(i, result);
		inputValues[index] = (res)?result[0]:255;
		if(!res) *readSuccess = false;
		index++;
	}
	for(i=0;i<msCount;i++) {
		bool res = LPFApi_getTiltMemsSensor(i, result);
		inputValues[index] = (res)?result[0]:255;
		inputValues[index+1] = (res)?result[1]:255;
		inputValues[index+2] = (res)?result[2]:255;
		if(!res) *readSuccess = false;
		index+=3;
	}
	return index; 
}
//skal være en call-back
int getNumberOfInputs() {
  	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	return dsCount+lsCount+tsCount+msCount;
}

//skal være en call-back
int getNumberOfOutputs() {
	int mCount  = LPFApi_getNumberOfMotors();
	int rgbCount  = LPFApi_getNumberOfRGBLights();
	int dispCount  = LPFApi_getNumberOfDisplays();
	return mCount+rgbCount+dispCount;
}



void controller_init() {
  LPFApi_init();
  LUI_init();
  kNN_init(&kNN_behavior, 1);
  kNN_init(&kNN_compound, 1);
  Playback_init(&playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
}
