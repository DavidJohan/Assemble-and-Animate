#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/Playback/Playback.h>

void Playback_init(Playback_t* process) {
	process->nextSet=0;
	process->nSets=0;
	process->startTimeMs = getLocalMsTime();
}

void Playback_startPlayback(Playback_t* process) {
	process->lastPlaybackIndex = 0;
	process->startTimeMs = getLocalMsTime();
}

void Playback_startRecording(Playback_t* process) {
	process->nextSet=0;
	process->nSets=0;
	process->startTimeMs = getLocalMsTime();
}

void Playback_clearRecordings(Playback_t* process) {
	process->nextSet=0;
	process->nSets=0;
	process->startTimeMs = 0;
}

void Playback_getOutput(Playback_t* process, signed char* output, int nOutputs) {
	if(process->nSets<=0) return;
  	int deliminator = (int)(process->sets[process->nSets-1].timeMs+1);
	long time = (getLocalMsTime() - process->startTimeMs)%deliminator;
	int i,j;
	for(i=0;i<nOutputs;i++) output[i] = 0;
	for(i=0;i<process->nSets-1;i++) {
		if(process->sets[i].timeMs<time){// && process->sets[i+1].timeMs>time) {
			for(j=0;j<nOutputs;j++) {
				output[j] = process->sets[i].output[j];
			}
		}
		else {
			#ifdef PRINTF
		 	ase_printf("Playback index %i at time %li ms (vs %li) -> (%i,%i) \n", i, time, process->sets[i].timeMs, output[0],output[1]);
			#endif
		 	ase_printf("X %li %li\n",process->sets[i].timeMs, time);
		 	//ase_printf("Playback index %i at time %li ms (vs %li) -> (%i,%i) \n", i, time, process->sets[i].timeMs, output[0],output[1]);
		 	return;
		}
	}
}
bool Playback_isFull(Playback_t* process) {
	return process->nSets>=(MAX_PLAYBACK_SETS-1);
}
bool Playback_record_if_novel(Playback_t* process, signed char* output, int nOutputs) {
	int index = (process->nextSet-1);
	if(index<0) return Playback_record(process, output, nOutputs);;
	for(int i=0;i<nOutputs;i++) {
		if(process->sets[index].output[i] != output[i]) {
			return Playback_record(process, output, nOutputs);
		}
	}
	return false;
}

bool Playback_record(Playback_t* process, signed char* output, int nOutputs){
	int i;
	#ifdef PRINTF
	ase_printf("Recording index %i at time %li ms -> (%i,%i) [%i/%i] \n", process->nextSet, getLocalMsTime()-process->startTimeMs, output[0], output[1], process->nSets, MAX_PLAYBACK_SETS);
	#endif
	if(process->nSets<MAX_PLAYBACK_SETS-1) {
		ase_printf("Recording index %i at time %li ms -> (%i,%i) [%i/%i] \n", process->nextSet, getLocalMsTime()-process->startTimeMs, output[0], output[1], process->nSets, MAX_PLAYBACK_SETS);
		for(i=0;i<MAX_PLAYBACK_OUTPUTS;i++) {
			if(i<nOutputs) {
				process->sets[process->nextSet].output[i] = output[i];
			}
			else {
				process->sets[process->nextSet].output[i] = 0;
			}
		}
		process->sets[process->nextSet].timeMs = getLocalMsTime()-process->startTimeMs;
		process->nextSet = process->nSets+1;
		if(process->nSets>=MAX_PLAYBACK_SETS) {
			return false;
		}
		else {
			process->nSets++;
			return true;
		}
	}
	else {
		ase_printf("Record full! %i\n",process->nSets);
		return false;
	}

}

