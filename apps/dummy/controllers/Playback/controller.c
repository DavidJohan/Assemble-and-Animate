#define	  ASE_CTRL

#include <stdio.h>
#include <stdlib.h>
#include <ase/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/Message.h>
#include <ase/Playback.h>

static Playback_t PlaybackProcess;

static void delay(long delayMs) {
	long start = getLocalMsTime();
	while(getLocalMsTime()<(start+delayMs));
}
void controller_init() {
	Playback_init(&PlaybackProcess);
	signed char output[1];
	int i;
	Playback_startRecording(&PlaybackProcess);
	printf("Recording:\n");
	for(i=0;i<20;i++) {
		output[0] = i%10;
		Playback_record(&PlaybackProcess,output, 1);
		printf("%i, ", (int)output[0]);
		if(i%10==0) printf("\n");
		delay(100);
	}
	printf("\n");


	Playback_startPlayback(&PlaybackProcess);
	printf("Playback:\n");
	for(i=0;i<100;i++) {
		Playback_getOutput(&PlaybackProcess,output, 1);
		printf("%i, ", (int)output[0]);
		if(i%10==0) printf("\n");
		delay(25);
	}
	//valgrind
}
