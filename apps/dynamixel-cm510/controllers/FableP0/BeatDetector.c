#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/targets/dynamixel.h>
#include "BeatDetector.h"

#define MAX_BEATS  	3

long beatHistory[MAX_BEATS];
int beatHistoryIndex;
int nBeats = 0;
bool gotBeat= false;
void micEvent() {
	long cTime = getLocalMsTime();
	//ase_printf(" %li: Mic Event \n",cTime);
	if(abs(beatHistory[beatHistoryIndex]-cTime)>100) {
		beatHistoryIndex = (beatHistoryIndex+1)%MAX_BEATS;
		if(nBeats<MAX_BEATS) nBeats++;
		beatHistory[beatHistoryIndex] = cTime;
		dynamixelApi_CM510_toggleLed(1);
		gotBeat=true;
	}
}
void BeatDetector_reset() {
	beatHistoryIndex = 0;
	beatHistory[0] = getLocalMsTime();
	nBeats=0;

}
void BeatDetector_init() {
	BeatDetector_reset();
	dynamixelApi_CM510_setMicEventFunction(micEvent);
	//TimerManager_createPeriodicTimer(750,0,micEvent);
}

long BeatDetector_lastBeatTime() {
	return beatHistory[beatHistoryIndex];
}
bool BeatDetector_gotBeat() {
	return gotBeat;
}

void BeatDetector_clearBeat() {
	gotBeat = false;
}

long BeatDetector_getBeatPeriod() {
	if(nBeats<2) return LONG_MAX;
	long prevT;
	int nPeriods = 0;
	long delaySum = 0;
	for(int i=0;i<nBeats;i++) {
		int index = (beatHistoryIndex-i);
		if(index<0) index = MAX_BEATS+index;
		if(i!=0) {
			delaySum += abs(beatHistory[index]-prevT);
			nPeriods++;
		}
		prevT = beatHistory[index];
	}
	return delaySum/nPeriods;
}

long BeatDetector_nextBeatTime() {
	if(nBeats<2) return LONG_MAX;
	return beatHistory[beatHistoryIndex] + BeatDetector_getBeatPeriod();
}
