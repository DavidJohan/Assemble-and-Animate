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
#include "controller.h"
#include "CM510Behaviors.h"
#include "BeatDetector.h"

static bool isSomeThingNearby(signed char* input, char nInputs) {
	bool near = false;
	for(int i=0;i<nInputs;i++) {
		if(getInputType(i)==DISTANCE) {
			if(input[i]>10) {
				near = true;
			}
		}
	}
	return near;
}

static bool isSomeTingVisible(signed char* input, char nInputs) {
	bool near = false;
	for(int i=0;i<nInputs;i++) {
		if(getInputType(i)==LONG_DISTANCE) {
			if(input[i]>10) {
				near = true;
			}
		}
	}
	return near;
}




void playback_start(void* data){
	Playback_startPlayback(data);
}

void playback_stop(void* data){};
void playback_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	signed char playbackOutput[nOutputs];
	Playback_getOutput((Playback_t*) data, playbackOutput, nOutputs);
	bool applyControl = false;
	for(int i=0;i<nOutputs;i++) {
		if(playbackOutput[i]!=0) {
			applyControl = true;
		}
	}
	if(applyControl) {
		for(int i=0;i<nOutputs;i++) {
			output[i] = playbackOutput[i];
		}
	}
}

void knn_behavior_start(void* data){};
void knn_behavior_stop(void* data){};
void knn_behavior_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	signed char knnInput[nInputs+1];
	signed char knnOutput[nOutputs];
	int nKnnInput  = LuiTraining_createKnnInput(input, nInputs, knnInput);
	kNN_getOutput((kNN_t*) data, knnInput, nKnnInput, knnOutput, nOutputs);
	//only apply control output if it is non-zero (subsumption friendly)
	bool applyControl = false;
	for(int i=0;i<nOutputs;i++) {
		if(knnOutput[i]!=0) {
			applyControl = true;
		}
	}
	if(applyControl) {
		for(int i=0;i<nOutputs;i++) {
			output[i] = knnOutput[i];
		}
	}
}

void escape_start(void* data) {
	CM510Behavior_escape_t* escape_data = (CM510Behavior_escape_t*) data;
	escape_data->dir=1;

}

void escape_stop(void* data) {}
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
  	CM510Behavior_escape_t* escape_data = (CM510Behavior_escape_t*) data;
  	/*if(isSomeThingNearby(input, nInputs)) {
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
  	}*/
  	long dTime = getLocalMsTime()-escape_data->startTime;
  	if(isSomeTingVisible(input, nInputs) && dTime>2000) {
  		escape_data->startTime = getLocalMsTime();
  		dTime = getLocalMsTime()-escape_data->startTime;
  		ase_printf("#play escape.wav 50 1\n");
  		escape_data->dir = (rand()%2==0)?-1:1;
	}
  	if(dTime<2000) {
  		if(dTime<750) {
  			for(int i=0;i<getNumberOfActuators();i++)  {
  				if(getOutputType(i) == WHEEL)  {
  					output[i] = escape_data->dir * 100;
  				}
  				else {
  					output[i] = escape_data->dir * 100;
  				}
  			}
  		}
  		else {
  			for(int i=0;i<getNumberOfActuators();i++)  {
  				if(getOutputType(i) == WHEEL)  {
  					if(i%2==0) output[i] = -100;
  					else output[i] = 100;
  				}
			}
  		}
  	}
}



void move_start(void* data) {
	//CM510Behavior_move_t* move_data = (CM510Behavior_move_t*) data;
	ase_printf("#play running_up_carpeted_stairs.wav 50 2\n");
}

void move_stop(void* data) {ase_printf("#play running_up_carpeted_stairs.wav 50 0\n");}
void move_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	//CM510Behavior_move_t* move_data = (CM510Behavior_move_t*) data;
  	float t = getLocalTime();
	for(int i=0;i<getNumberOfActuators();i++) {
		int sign = (i%2==0)?-1:1;
		output[i] += sin(6.28f*t/(6.0f+i))*sign*50;
		if(output[i]>100) output[i] = 100;
		if(output[i]<-100) output[i] = -100;
	}
	if(isSomeThingNearby(input, nInputs)) {
		for(int i=0;i<getNumberOfActuators();i++) {
			output[i] = -1*output[i];
		}
	}
}

void seek_start(void* _data) {
	CM510Behavior_seek_t* data = (CM510Behavior_seek_t*) _data;
	ase_printf("#play running_up_carpeted_stairs.wav 50 2\n");
	data->playingSound = true;
}

void seek_stop(void* _data) {
	CM510Behavior_seek_t* data = (CM510Behavior_seek_t*) _data;
	ase_printf("#play running_up_carpeted_stairs.wav 50 0\n");
	data->playingSound = false;
}
void seek_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data) {
	CM510Behavior_seek_t* data = (CM510Behavior_seek_t*) _data;
  	float t = getLocalTime();
	for(int i=0;i<getNumberOfActuators();i++) {
		int sign = (i%2==0)?-1:1;
		if(getOutputType(i)==WHEEL) {
			output[i] = -(sin(6.28f*t/(6.0f+i))+1)*sign*60/2; //only forward
		}
		else {
			output[i] = sin(6.28f*t/(6.0f+i))*sign*50;
		}

		if(output[i]>100) output[i] = 100;
		if(output[i]<-100) output[i] = -100;
	}
	if(isSomeThingNearby(input, nInputs) || (getLocalMsTime()-data->detectTime)<2000) {
		//found someting behavior
		if(data->detectTime==0 ) {
			data->detectTime=getLocalMsTime();
		}
		for(int i=0;i<getNumberOfActuators();i++) {
			if((getLocalMsTime()-data->detectTime)<750) {
				output[i] = 0;
			}
			else {
				if(getOutputType(i) == WHEEL)  {
					if(data->turnDir) output[i] = 70;
					else output[i] = -70;
				}
				else {
					output[i] = 0;
				}
			}
		}
		if(data->playingSound) {
			ase_printf("#play running_up_carpeted_stairs.wav 50 0\n");
			ase_printf("#play found.wav 50 1\n");
			data->playingSound = false;
			data->turnDir = rand()%2==0;
		}
	}
	else {
		//nothing found
		if(!data->playingSound) {
			ase_printf("#play running_up_carpeted_stairs.wav 50 2\n");
			data->playingSound = true;
		}
		data->detectTime = 0;
	}
}



void bird_song_start(void* data) {ase_printf("#play papegoje.wav 50 2\n");}
void bird_song_stop(void* data) {ase_printf("#play papegoje.wav 50 0\n");}
void bird_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}

void bee_song_start(void* data) {ase_printf("#play bee.wav 50 2\n");}
void bee_song_stop(void* data) {ase_printf("#play bee.wav 50 0\n");}
void bee_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}


void monkey_song_start(void* data) {ase_printf("#play abe.wav 50 2\n");}
void monkey_song_stop(void* data) {ase_printf("#play abe.wav 50 0\n");}
void monkey_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {}


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
				else {
					//no sound, stop dancing
					dance_data->started = false;
					//output[i] = 0;
				}
				if(dynamixelApi_isWheelMode(i)) {
					if(output[i]>0)output[i] = dance_data->dofs[i].range;
					if(output[i]<0)output[i] = -dance_data->dofs[i].range;
				}
			}
			else {
				//output[i] = 0;
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
	float t = getLocalTime();
	for(int i=0;i<getNumberOfActuators();i++) {
		int sign = (i%2==0)?-1:1;
		output[i] += sin(6.28f*t/(3.0f+i))*sign*100;
		if(output[i]>100) output[i] = 100;
		if(output[i]<-100) output[i] = -100;
	}
}

void play_dead_start(void* _data) {
	CM510Behavior_play_dead_t* data = (CM510Behavior_play_dead_t*) _data;
	data->started = false;
	data->ended = false;
	BeatDetector_clearBeat();
}
void play_dead_stop(void* _data){}
void play_dead_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data){
	CM510Behavior_play_dead_t* data = (CM510Behavior_play_dead_t*) _data;
	if(BeatDetector_gotBeat() && !data->started) {
		data->startTime = getLocalMsTime();
		data->started = true;
		data->ended = false;
		ase_printf("#play playdead.wav 50 1\n");
	}
	if(data->started) {
		long deadTime = 5000-(getLocalMsTime()-data->startTime);
		if(deadTime>0) {
			output[0] = 100*((float)deadTime)/5000.0f;
			for(int i=1;i<getNumberOfActuators();i++) {
				output[i] = 0;
			}
		}
		else {
			if(!data->ended)  {
				ase_printf("#play theEnd.wav 50 1\n");
				data->ended = true;
			}
			if(deadTime>-5000) {
				for(int i=0;i<getNumberOfActuators();i++) {
					if(getOutputType(i)==WHEEL) output[i] = 0;
					else output[i] = 100;
				}
			}
			else if(deadTime>-7000) { //come to live
				for(int i=0;i<getNumberOfActuators();i++) {
					output[i] = 0;
				}
			}
			else { //ready to die again
				data->started = false;
				BeatDetector_clearBeat();
			}
		}
	}
}

void attack_start(void* _data){
	CM510Behavior_attack_t* data = (CM510Behavior_attack_t*) _data;
	data->startTime = getLocalMsTime();
	data->itteration = 0;
	data->playingAngrySound = false;
}
void attack_stop(void* _data){
	ase_printf("#play attack.wav 50 0\n");
	ase_printf("#play angry.wav 50 0\n");
}
void attack_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data){
	CM510Behavior_attack_t* data = (CM510Behavior_attack_t*) _data;
	//if(isSomeThingNearby(input, nInputs)) { //then attack
	if(isSomeTingVisible(input, nInputs)) { //then attack
		if((getLocalMsTime()-data->startTime)<2000) {
			if(!data->playingAngrySound) {
				ase_printf("#play angry.wav 50 2\n");
				data->playingAngrySound = true;
			}
			for(int i=0;i<getNumberOfActuators();i++) {
				int sign = (i%2==0)?1:-1; //forward
				int dir = (data->itteration%2==0)?1:-1;
				output[i] = sign*dir*100;
			}
		}
		else if((getLocalMsTime()-data->startTime)<2600) {
			if(data->playingAngrySound) {
				ase_printf("#play attack.wav 50 1\n");
				ase_printf("#play angry.wav 50 0\n");
				data->playingAngrySound = false;
			}
			for(int i=0;i<getNumberOfActuators();i++) {
				int sign = (i%2==0)?1:-1; //forward
				if(i<2) output[i] = sign*100;
				else output[i] = 0;
			}
		}
		else if((getLocalMsTime()-data->startTime)<2800) {
			for(int i=0;i<getNumberOfActuators();i++) {
				int sign = (i%2==0)?1:-1; //backward
				if(i<2) output[i] = -sign*100;
				else output[i] = 0;
			}
		}
		else if((getLocalMsTime()-data->startTime)<3300) {
			for(int i=0;i<getNumberOfActuators();i++) {
				output[i] = 0;
			}
		}
		else {
			data->startTime = getLocalMsTime(); //restart
		}
	}
	else {
		data->startTime = getLocalMsTime();
		if(data->playingAngrySound) {
			ase_printf("#play angry.wav 50 0\n");
			data->playingAngrySound = false;
		}
	}
	data->itteration++;
}

void hide_start(void* _data){
	CM510Behavior_hide_t* data = (CM510Behavior_hide_t*) _data;
	data->playingSound = false;
}
void hide_stop(void* _data){
	CM510Behavior_hide_t* data = (CM510Behavior_hide_t*) _data;
	ase_printf("#play playDrum.wav 50 0\n");
	data->playingSound = false;
}
void hide_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data){
	CM510Behavior_hide_t* data = (CM510Behavior_hide_t*) _data;
	if(!isSomeThingNearby(input, nInputs)) { //then hide
		for(int i=0;i<getNumberOfActuators();i++) {
			int sign = (i%2==0)?1:-1; //forward
			if(getOutputType(i)==WHEEL) output[i] = sign*100;
			else output[i] = sin(6.28f*getLocalTime())*sign*20;
		}
		if(!data->playingSound) {
			ase_printf("#play playDrum.wav 50 2\n");
			data->playingSound = true;
		}
	}
	else {
		if(data->playingSound) {
			ase_printf("#play playDrum.wav 50 0\n");
			data->playingSound = false;
		}
		for(int i=0;i<getNumberOfActuators();i++) {
			if(getOutputType(i)==WHEEL) output[i] = 0;
			else output[i] = 100;
		}
	}
}
int crop(int val, int min, int max) {
	if(val<min) return min;
	if(val>max) return max;
	return val;
}
void follow_start(void* _data) {
	CM510Behavior_follow_t* data = (CM510Behavior_follow_t*) _data;
	data->playingSound = false;
}
void follow_stop(void* _data) {
	CM510Behavior_follow_t* data = (CM510Behavior_follow_t*) _data;
	data->playingSound = false;
	ase_printf("#play fly0.wav 50 0\n");
}
void follow_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data) {
	CM510Behavior_follow_t* data = (CM510Behavior_follow_t*) _data;
	bool follow = false;
	for(int i=0;i<nInputs;i++) {
		if(getInputType(i)==DISTANCE) {
			if(input[i]>5) {
				follow = true;
			}
		}
	}
	if(follow) {
		if(!data->playingSound) {
			ase_printf("#play fly0.wav 50 1\n");
			data->playingSound = true;
		}
		int sIndex=1;
		for(int i=nOutputs;i>=0;i--) {
			if(getOutputType(i)==WHEEL) {
				bool found = false;
				int sign = (i%2==0)?1:-1; //forward
				while(!found && sIndex<nInputs) {
					if(getInputType(sIndex)==DISTANCE) {
						if(input[sIndex]>5 && input[sIndex]<30) { //if there is someting
							output[i] = crop(15*sign*(50-input[sIndex]), -100, 100);
						}
						else {
							output[i] = 0;
						}
						found = true;
						//ase_printf("%i -> %i (%i)\n", i, output[i],input[sIndex]);
					}
					sIndex++;
				}
			}
			else {
				//slow movemet with head;
				output[i] = 0;
			}
		}
	}
	else {
		if(data->playingSound) {
			ase_printf("#play fly0.wav 50 0\n");
			data->playingSound = false;
		}
	}
}


void play_start(void* _data){
	CM510Behavior_play_t* data = (CM510Behavior_play_t*) _data;
	data->startTime = getLocalMsTime();
}
void play_stop(void* _data){}
void play_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* _data){
	CM510Behavior_play_t* data = (CM510Behavior_play_t*) _data;
	if(data->actSurprised) {
		for(int i=0;i<getNumberOfActuators();i++) {
			int inputIndex = (i+1)%(nInputs-1);
			output[i] = 10*input[inputIndex]*data->randomDirs[i];
			output[i] = (output[i]>100)?100:output[i];
		}
		if(isSomeThingNearby(input, nInputs)) {
			data->startTime = getLocalMsTime();
			if((rand()%100)>80) ase_printf("#play laught.wav 50 1\n");
		}
		if((getLocalMsTime()-data->startTime)>2000) {
			data->actSurprised = false;
		}
	}
	else {
		if(isSomeThingNearby(input, nInputs)) {
			ase_printf("#play laught.wav 50 1\n");
			data->actSurprised = true;
			data->startTime = getLocalMsTime();
			for(int i=0;i<nOutputs;i++) {
				data->randomDirs[i] = (rand()%2==0)?-1:1;
			}
		}
		/*if(BeatDetector_gotBeat()) {
			BeatDetector_clearBeat();
			ase_printf("#play laught.wav 50 1\n");
			data->actSurprised = true;
			data->startTime = getLocalMsTime();
			BeatDetector_clearBeat();
			for(int i=0;i<nOutputs;i++) {
				data->randomDirs[i] = (rand()%2==0)?-1:1;
			}
		}*/
	}
}

