#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiTraining.h>
#include "CM510Behaviors.h"


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
	CM510Behavior_escape_t* escape_data = (CM510Behavior_escape_t*) data;
	escape_data->dir=1;
}
void escape_stop(void* data) {}
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
  	//CM510Behavior_escape_t* escape_data = (CM510Behavior_escape_t*) data;
  	int irVal = input[0];
  	int pos, pos2;
  	if(irVal>10) {
  		pos = 300;
  		pos2 = 512;
  		output[0] = pos; output[2] = pos; output[4] = pos;
  		output[1] = pos2; output[3] = pos2; output[5] = pos2;
  		ase_printf("#play escape.wav 50 1\n");
  	}
  	else {
  		pos = 450;
  		pos2 = 850;
  		output[0] = pos; output[2] = pos; output[4] = pos;
  		output[1] = pos2; output[3] = pos2; output[5] = pos2;
  	}
  	ase_printf("%i\n",irVal);
}


void move_start(void* data) {
	CM510Behavior_move_t* move_data = (CM510Behavior_move_t*) data;
	move_data->lastSoundTime=0;
	ase_printf("#play running_up_carpeted_stairs.wav 50 2\n");
}

void move_stop(void* data) {ase_printf("#play running_up_carpeted_stairs.wav 50 0\n");}
void move_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	/*CM510Behavior_move_t* move_data = (CM510Behavior_move_t*) data;
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
	}*/
}

void geiger_start(void* data) {
	CM510Behavior_geiger_t* geiger_data = (CM510Behavior_geiger_t*) data;
	geiger_data->lastSoundTime=0;
	geiger_data->delay=500;
	geiger_data->lastIntensity=0;
}
void geiger_stop(void* data) {}
void geiger_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	/*CM510Behavior_geiger_t* geiger_data = (CM510Behavior_geiger_t*) data;
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
	}*/
}

void bird_song_start(void* data) {ase_printf("#play papegoje.wav 50 2\n");}
void bird_song_stop(void* data) {ase_printf("#play papegoje.wav 50 0\n");}
void bird_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}

void bee_song_start(void* data) {ase_printf("#play bee.wav 50 2\n");}
void bee_song_stop(void* data) {ase_printf("#play bee.wav 50 0\n");}
void bee_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}



void dance_start(void* data) {
  CM510Behavior_dance_t* dance_data = (CM510Behavior_dance_t*) data;
  int i;
  for(i=0;i<5;i++)  dance_data->role[i] = i;
  dance_data->lastChangeTime = 0;

}

void dance_stop(void* data) {}
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	//CM510Behavior_dance_t* dance_data = (CM510Behavior_dance_t*) data;
	float t = getLocalTime();
	int pos = 200*sin(t/10.0f)+512;
	int pos2 = 200*sin(-t/10.0f)+512+100;
	if(pos2<512) pos2=512;
	output[0] = pos; output[2] = pos; output[4] = pos;
	output[1] = pos2; output[3] = pos2; output[5] = pos2;
}

void fly_start(void* data) {
  /*CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 2\n");
  fly_data->tilt_play = false;*/
}

void fly_stop(void* data) {
  CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 0\n");
  ase_printf("#play comedy_twirly_whirly.wav 50 0\n");
  fly_data->tilt_play = false;
}
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	/*CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
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
	}*/
}

