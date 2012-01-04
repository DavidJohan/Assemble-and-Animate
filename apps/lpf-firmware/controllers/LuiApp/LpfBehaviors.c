#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/non-public/lpf.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiTraining.h>
#include "LpfBehaviors.h"




void copyIndex(int from, int to, signed char* result) {
	int i;
  	for(i=from;i<to;i++) {
		result[i-from] = i;		
	}	
}
int selectMotors(signed char* result) {
	int nMotors = LPFApi_getNumberOfMotors();
	copyIndex(0, nMotors, result);
	return nMotors;
}

int selectDetectSensors(signed char* result) {
	int nDetect = LPFApi_getNumberOfDetectSensors();
	copyIndex(0, nDetect, result);
	return nDetect;
}

int selectMemsTilt(signed char* result) {
  	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	int from = dsCount+lsCount+tsCount;
	int to = dsCount+lsCount+tsCount+msCount;
	copyIndex(from, to, result);
	return msCount;
}

int selectTilt(signed char* result) {
  	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	int from = dsCount+lsCount;
	int to = dsCount+lsCount+tsCount;
	copyIndex(from, to, result);
	return tsCount;
}


void playback_start(void* data){
	Playback_startPlayback(data);
}
void playback_stop(void* data){};
void playback_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	Playback_getOutput((Playback_t*) data, output, nOutputs);
}

void knn_behavior_start(void* data){};
void knn_behavior_stop(void* data){};
void knn_behavior_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	signed char knnInput[10];
	int nKnnInput  = LuiTraining_createKnnInput(input, nInputs, knnInput);
	kNN_getOutput((kNN_t*) data, knnInput, nKnnInput, output, nOutputs);
}

void escape_start(void* data) {
	LpfBehavior_escape_t* escape_data = (LpfBehavior_escape_t*) data;
	escape_data->dir=1;
}
void escape_stop(void* data) {}
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	//adaptive - able to detect approach instead of escape?
  	LpfBehavior_escape_t* escape_data = (LpfBehavior_escape_t*) data;
  	signed char detectSensors[10];
  	int nDetectSensors = selectDetectSensors(detectSensors);
	bool escape = false;
	
	int i;
	for(i=0;i<nDetectSensors;i++) {
	  	char val = input[detectSensors[i]];
		if(val<50) {
			escape = true;
			ase_printf("detect sensor = %i\n", input[detectSensors[i]]);
		}
	}
	
	if(escape) {  //play escape sound - send to PC with small wireless loud speaker
		if(escape_data->startTime==-1||(escape_data->startTime+1000)< getLocalMsTime()) {
			escape_data->dir = -1*escape_data->dir;
			escape_data->startTime = getLocalMsTime();
			ase_printf("#play escape.wav 50 1\n");
	  	}
		signed char motors[10];
		int nMotors = selectMotors(motors);
		
		int i;
		for(i=0;i<nMotors;i++) {
			int sign = (i%2==0)?-1:1;
			output[motors[i]] = escape_data->dir*sign*100;
		}
		if(escape_data->dir==1) ase_printf("Escape forward %i sec\n", getLocalMsTime()-escape_data->startTime);
		if(escape_data->dir==-1) ase_printf("Escape backwards %i sec\n", getLocalMsTime()-escape_data->startTime);
	}
	else {
		escape_data->startTime = -1;
	}
}


void move_start(void* data) {
	LpfBehavior_move_t* move_data = (LpfBehavior_move_t*) data;
	move_data->lastSoundTime=0;
	ase_printf("#play running_up_carpeted_stairs.wav 50 2\n");
}

void move_stop(void* data) {ase_printf("#play running_up_carpeted_stairs.wav 50 0\n");}
void move_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
  	LpfBehavior_move_t* move_data = (LpfBehavior_move_t*) data;
  	signed char motors[10];
	int i, nMotors = selectMotors(motors);
	float t = getLocalTime();
	for(i=0;i<nMotors;i++) {
		int sign = (i%2==0)?-1:1;
		output[motors[i]] += sin(6.28f*t/(6.0f+i))*sign*50;
		if(output[motors[i]]>100) output[motors[i]] = 100;
		if(output[motors[i]]<-100) output[motors[i]] = -100;
	}
	if(move_data->lastSoundTime+5000 < getLocalMsTime()) {
		move_data->lastSoundTime = getLocalMsTime();
	}
}

void geiger_start(void* data) {
	LpfBehavior_geiger_t* geiger_data = (LpfBehavior_geiger_t*) data;
	geiger_data->lastSoundTime=0;
	geiger_data->delay=500;
	geiger_data->lastIntensity=0;
}
void geiger_stop(void* data) {}
void geiger_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	LpfBehavior_geiger_t* geiger_data = (LpfBehavior_geiger_t*) data;
  	signed char detectSensors[10];
	int max_intensity = 0;
  	int i, nDetectSensors = selectDetectSensors(detectSensors);
  	for(i=0;i<nDetectSensors;i++) {
		ase_printf("detect sensor = %i\n", input[detectSensors[i]]);
		int intensity;
		if(input[detectSensors[i]]<0) intensity = 0;
		else if(input[detectSensors[i]]<11) intensity = 8;
		else if(input[detectSensors[i]]<22) intensity = 7;
		else if(input[detectSensors[i]]<33) intensity = 6;
		else if(input[detectSensors[i]]<44) intensity = 5;
		else if(input[detectSensors[i]]<55) intensity = 4;
		else if(input[detectSensors[i]]<66) intensity = 3;
		else if(input[detectSensors[i]]<77) intensity = 2;
		else if(input[detectSensors[i]]<88) intensity = 1;
		else intensity = 0;
		if(intensity>max_intensity) max_intensity = intensity;
	}
	
	if(geiger_data->lastSoundTime+geiger_data->delay < getLocalMsTime()|| geiger_data->lastIntensity!=max_intensity) {
		geiger_data->lastSoundTime = getLocalMsTime();
		geiger_data->delay = rand()%1000+1000;
		geiger_data->lastIntensity = max_intensity;
		if(max_intensity==0)	;
		if(max_intensity==1)	ase_printf("#play geiger1.wav 50 1\n");
		if(max_intensity==2)	ase_printf("#play geiger1.wav 50 1\n");
		if(max_intensity==3)	ase_printf("#play geiger2.wav 50 1\n");
		if(max_intensity==4)	ase_printf("#play geiger2.wav 50 1\n");
		if(max_intensity==5)	ase_printf("#play geiger3.wav 50 1\n");
		if(max_intensity==6)	ase_printf("#play geiger3.wav 50 1\n");
		if(max_intensity==7)	ase_printf("#play geiger4.wav 50 1\n");
		if(max_intensity==8)	ase_printf("#play geiger4.wav 50 1\n");	
	}
}

void bird_song_start(void* data) {ase_printf("#play papegoje.wav 50 2\n");}
void bird_song_stop(void* data) {ase_printf("#play papegoje.wav 50 0\n");}
void bird_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}

void bee_song_start(void* data) {ase_printf("#play bee.wav 50 2\n");}
void bee_song_stop(void* data) {ase_printf("#play bee.wav 50 0\n");}
void bee_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}



void dance_start(void* data) {
  LpfBehavior_dance_t* dance_data = (LpfBehavior_dance_t*) data;
  int i;
  for(i=0;i<5;i++)  dance_data->role[i] = i;
  dance_data->lastChangeTime = 0;
  
}

void dance_stop(void* data) {}
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	LpfBehavior_dance_t* dance_data = (LpfBehavior_dance_t*) data;
	signed char motors[5];
	int i, nMotors = selectMotors(motors);
	float t = getLocalTime();
	
	for(i=0;i<nMotors;i++) { 
	  	if(dance_data->role[i]==0) output[motors[i]] = 0;
		if(dance_data->role[i]==1) output[motors[i]] = 100*sin(6.28f*t/(0.733f));
		if(dance_data->role[i]==2) output[motors[i]] = 100*sin(6.28f*t/(1.466f));
		if(dance_data->role[i]==3) output[motors[i]] = -75;
		if(dance_data->role[i]==4) output[motors[i]] = 75;
		
		if(output[motors[i]]>100) output[motors[i]] = 100;
		if(output[motors[i]]<-100) output[motors[i]] = -100;
	}
	if(dance_data->lastChangeTime+2932 < getLocalMsTime()) {
		dance_data->lastChangeTime  = getLocalMsTime();
		char temp = dance_data->role[0];
		for(i=0;i<4;i++) {
		  dance_data->role[i] = dance_data->role[i+1];
		}
		dance_data->role[4] = temp;		
	}
}



void fly_start(void* data) {
  LpfBehavior_fly_t* fly_data = (LpfBehavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 2\n");
  fly_data->tilt_play = false;
}

void fly_stop(void* data) {
  LpfBehavior_fly_t* fly_data = (LpfBehavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 0\n");
  ase_printf("#play comedy_twirly_whirly.wav 50 0\n");
  fly_data->tilt_play = false;
}
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	LpfBehavior_fly_t* fly_data = (LpfBehavior_fly_t*) data;
	signed char motors[10];
	int i, nMotors = selectMotors(motors);
	float t = getLocalTime();
	for(i=0;i<nMotors;i++) {
		int sign = (i%2==0)?-1:1;
		output[motors[i]] += sin(6.28f*t/(3.0f+i))*sign*100;
		if(output[motors[i]]>100) output[motors[i]] = 100;
		if(output[motors[i]]<-100) output[motors[i]] = -100;
	}
	//output[motors[0]] = 100; //mund
	//output[motors[1]] = 100; //vinger
	//output[motors[2]] = 100; //hals
	
	signed char tilt[10];
	bool tilted = false;
	int maxTilt = 0;
	int nTilt = selectTilt(tilt);
	for(i=0;i<nTilt;i++) {
		ase_printf("Tilt val: %i \n", input[tilt[i]]);
		if(input[tilt[i]]>=20) {
			tilted = true;
		}
		if(input[tilt[i]]>maxTilt) {
			maxTilt = input[i];
		}
	}
	if(tilted) {
		if(!fly_data->tilt_play) {
			ase_printf("#play comedy_twirly_whirly.wav 50 2\n");
			fly_data->tilt_play = true;
		}
		signed char motors[10];
		int i, nMotors = selectMotors(motors);
		for(i=0;i<nMotors;i++) {
			output[motors[i]] = 0;
		}
	}
	else {
		if(!fly_data->tilt_play) {
			ase_printf("#play comedy_twirly_whirly.wav 50 0\n");
			fly_data->tilt_play = false;
		}
	}
}

void sea_start(void* data) {}
void sea_stop(void* data) {}
void sea_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
  	signed char tilt[10];
	int nTilt = selectTilt(tilt);
	float t = getLocalTime();
	int i;
	bool tilted = false;
	int maxTilt = 0;
	for(i=0;i<nTilt;i++) {
	  ase_printf("Tilt val: %i \n", input[tilt[i]]);
		if(input[tilt[i]]>=20) {
			tilted = true;
		}
		if(input[tilt[i]]>maxTilt) {
			maxTilt = input[i];
		}
	}
	if(tilted) {
		ase_printf("#play comedy_twirly_whirly.wav 50 1\n");
		signed char motors[10];
		int i, nMotors = selectMotors(motors);
		for(i=0;i<nMotors;i++) {
			output[motors[i]] = 0;
		}
	}
}

/*void LpfBehaviors_installBehaviors(Subsumption_t* SubsumptionProcess) {
	Subsumption_addBehavior(98, 0, 0, &playback_data, playback_start, playback_act, playback_stop, SubsumptionProcess);
	Subsumption_addBehavior(99, 0, 0, &kNN_behavior, knn_behavior_start, knn_behavior_act, knn_behavior_stop, SubsumptionProcess);
	
	Subsumption_addBehavior(1, 0, 0, &escape_data, escape_start, escape, escape_stop, SubsumptionProcess);
	Subsumption_addBehavior(2, 0, 0, &move_data, move_start, moveAround, move_stop, SubsumptionProcess);
	Subsumption_addBehavior(3, 0, 0, &geiger_data, geiger_start, geiger, geiger_stop, SubsumptionProcess);
	Subsumption_addBehavior(4, 0, 0, &bird_data, bird_song_start, bird_song, bird_song_stop, SubsumptionProcess);
	Subsumption_addBehavior(5, 0, 0, &bee_data, bee_song_start, bee_song, bee_song_stop, SubsumptionProcess);
	Subsumption_addBehavior(6, 0, 0, &fly_data, fly_start, fly, fly_stop, SubsumptionProcess);
	Subsumption_addBehavior(7, 0, 0, &dance_data, dance_start, dance, dance_stop, SubsumptionProcess);
}*/


