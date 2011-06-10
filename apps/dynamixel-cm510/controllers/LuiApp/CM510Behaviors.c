#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiTraining.h>
#include "CM510Behaviors.h"
#include "BeatDetector.h"


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
	escape_data->startTime = getLocalMsTime();
}

void escape_stop(void* data) {}
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
  	CM510Behavior_escape_t* escape_data = (CM510Behavior_escape_t*) data;
  	int irVal = input[0];
  	if(irVal>10) {
  		if((escape_data->startTime+750)<getLocalMsTime()) {
  			if(escape_data->dir==1) escape_data->dir = -1;
  			else escape_data->dir = 1;
  			escape_data->startTime = getLocalMsTime();
  			ase_printf("#play escape.wav 50 1\n");
  		}
  		for(int i=0;i<getNumberOfActuators();i++)  {
  			if(i%2==0) output[i] = -escape_data->dir*100;
  			if(i%2==1) output[i] = escape_data->dir*100;
  		}
  	}
  	else {
  		for(int i=0;i<getNumberOfActuators();i++)  {
			//output[i] = 0;
		}
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
	CM510Behavior_move_t* move_data = (CM510Behavior_move_t*) data;
  	float t = getLocalTime();
	for(int i=0;i<getNumberOfActuators();i++) {
		int sign = (i%2==0)?-1:1;
		output[i] += sin(6.28f*t/(6.0f+i))*sign*50;
		if(output[i]>100) output[i] = 100;
		if(output[i]<-100) output[i] = -100;
	}
	if(move_data->lastSoundTime+5000 < getLocalMsTime()) {
		move_data->lastSoundTime = getLocalMsTime();
	}
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


int period = 100;


//sound control
//parameters in need of control: amplitude, frequency, phase, offset or raw POS per DOF
//parameters in sound:
//	+ 1 tone controls 1 DOF, how many can we differenciate?
//	+ the rhythm of the tone control the frequency and phase (implies osscillating control?)
//  + the loudness controls the amplitude
//Algorithm:
//	+ FFT on a small window looking for tones in the interval 100 - 3000 Hz
//  + Each tone is treated separately (tone, amplitude, time, delta time)

void dance_randomize(CM510Behavior_dance_t* dance_data, int index) {
	dance_data->dofs[index].active = rand()%4!=0;
	dance_data->dofs[index].cycle = rand()%2+1;
	dance_data->dofs[index].phase = rand()%2;
	dance_data->dofs[index].range = rand()%25+25;
}

void dance_start(void* data) {
  CM510Behavior_dance_t* dance_data = (CM510Behavior_dance_t*) data;
  int i;
  for(i=0;i<getNumberOfActuators();i++)  {
	  dance_randomize(dance_data, i);
	  dance_data->dofs[i].periodTime = 1000;
	  dance_data->dofs[i].periodStartTime = getLocalMsTime();
  }
  BeatDetector_reset();
  dance_data->started = false;
 // dance_data->nextBeatTime = LONG_MAX;
}

void dance_stop(void* data) {}
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	CM510Behavior_dance_t* dance_data = (CM510Behavior_dance_t*) data;
	if(BeatDetector_gotBeat()) {
		BeatDetector_clearBeat();
		dynamixelApi_CM510_toggleLed(0);
		long beatPeriod = BeatDetector_getBeatPeriod();
		if(beatPeriod!=LONG_MAX) {
			dance_data->started = true;
		}
		else {
			return;
		}
		bool randomize = rand()%4==0;
		for(int i=0;i<getNumberOfActuators();i++)  {
			if(randomize) dance_randomize(dance_data, i);
			float time = ((float)(getLocalMsTime() - dance_data->dofs[i].periodStartTime))/dance_data->dofs[i].periodTime;
			if(time>0.9f) {
				dance_data->dofs[i].periodTime = dance_data->dofs[i].cycle * beatPeriod;
				dance_data->dofs[i].periodStartTime = getLocalMsTime();
			}
		}
	}
	if(dance_data->started) {
		for(int i=0;i<getNumberOfActuators();i++)  {
			float A  = (float)dance_data->dofs[i].range;
			float sign = (dance_data->dofs[i].phase==0)?-1:1;
			float pi = 3.14159f;
			float time = ((float)(getLocalMsTime() - dance_data->dofs[i].periodStartTime))/dance_data->dofs[i].periodTime;
			if(dance_data->dofs[i].active) {
				if(time<=1.0f) output[i] = (int)(A*sin(2*pi*time + sign*pi/2));
				else if(time<=1.5f) output[i] = (int)(A*sin(2*pi*1.0 + sign*pi/2));
				else output[i] = 0;
				if(dynamixelApi_isWheelMode(i)) {
					if(output[i]>0)output[i] = dance_data->dofs[i].range;
					if(output[i]<0)output[i] = -dance_data->dofs[i].range;
				}
			}
			else {
				output[i] = 0;
			}
		}
	}
}

void fly_start(void* data) {
  CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 2\n");
  fly_data->tilt_play = false;
}

void fly_stop(void* data) {
  CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
  ase_printf("#play fly0.wav 50 0\n");
  ase_printf("#play comedy_twirly_whirly.wav 50 0\n");
  fly_data->tilt_play = false;
}
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	CM510Behavior_fly_t* fly_data = (CM510Behavior_fly_t*) data;
	float t = getLocalTime();
	for(int i=0;i<getNumberOfActuators();i++) {
		int sign = (i%2==0)?-1:1;
		output[i] += sin(6.28f*t/(3.0f+i))*sign*100;
		if(output[i]>100) output[i] = 100;
		if(output[i]<-100) output[i] = -100;
	}
}

