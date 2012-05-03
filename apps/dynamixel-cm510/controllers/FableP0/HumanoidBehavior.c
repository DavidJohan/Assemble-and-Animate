#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>
#include "controller.h"
#include "BeatDetector.h"
#include "HumanoidBehavior.h"

void humanoid_start(void* data){
	((CM510Behavior_humanoid_t*)data)->playing = false;
}
signed char output_old[6];
float ignoretime = 0;
float dancetime = 0;
int doDanceItt=0;
void humanoid_stop(void* data) {}
void humanoid_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data) {
	float t = getLocalTime();
	if(getInputType(0)==LONG_DISTANCE && input[0]>10) {
		if(((CM510Behavior_humanoid_t*)data)->playing == false) {
			ase_printf("#play papegoje.wav 50 2\n");
			((CM510Behavior_humanoid_t*)data)->playing = true;
			dancetime = t;
			for(int i=0;i<getNumberOfActuators();i++) {
				dynamixelApi_setCompliance(i,1,254);
			}
		}
		if((dancetime + 5)>t){
			output[0] = 50*sin(4*t)-50;
			output[1] = -100;
			output[2] = -50;
			output[3] = 0;
		}
		else if((dancetime + 10)>t){
			output[0] = 50;
			output[1] = 0;
			output[2] = -50*sin(4*t)+50;
			output[3] = -100;
		}
		else if((dancetime + 15)>t){
			output[0] = 50*sin(4*t)-50;
			output[1] = -100;
			output[2] = -50*sin(4*t)+50;
			output[3] = -100;
		}
		else {
			dancetime = t;
		}


		for(int i=0;i<nOutputs;i++) output_old[i] = output[i];
		doDanceItt++;
		if(doDanceItt>10) doDanceItt=10;
		return;
	}
	else if(getInputType(0)==LONG_DISTANCE && input[0]<=10){
		if(doDanceItt>0) {
			for(int i=0;i<nOutputs;i++) {
				output[i] = 127;
			}
			doDanceItt--;
			return;
		}
		if(((CM510Behavior_humanoid_t*)data)->playing == true) {
			ase_printf("#play papegoje.wav 50 0\n");
			((CM510Behavior_humanoid_t*)data)->playing = false;
			ignoretime = t+1.0f;
		}
		if(ignoretime>t) {
			for(int i=0;i<nOutputs;i++) {
				output[i] = 127;
			}
			BeatDetector_clearBeat();
			return;
		}
		if(BeatDetector_gotBeat()) {
			BeatDetector_clearBeat();
			for(int i=0;i<getNumberOfActuators();i++) {
				dynamixelApi_setCompliance(i,1,32);
			}
			ase_printf("#play laught.wav 50 1\n");
			for(int i=0;i<nOutputs;i++) {
				if(rand()%2==0) output[i] = output_old[i] + 25;
				else output[i] = output_old[i] - 25;
			}
			for(int i=0;i<nOutputs;i++) output_old[i] = output[i];
			ignoretime = t+1.0f;
			return;
		}
	}
	for(int i=0;i<nOutputs;i++) {
		output[i] = 127;
	}


}

