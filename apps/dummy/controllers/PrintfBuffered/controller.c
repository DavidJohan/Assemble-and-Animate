#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <ase/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/EventManager/EventManager.h>

Timer_t* printTimer;
Timer_t* readTimer;

static char buffer[100];
static int buf_write_index=0;
static int buf_read_index=0;
static int buf_size=0;
static char buf[100];

#define buf_printf(format, ...)  {\
	int length = sprintf(buf,format, ##__VA_ARGS__);\
	if(length+buf_size>100) {\
		printf("Error: Buffer overflow\n");\
		return;\
	}\
	int i;\
	for(i=0;i<length;i++) {\
		buffer[buf_write_index]=buf[i];\
		buf_write_index = (buf_write_index+1)%100;\
		buf_size++;\
	}\
}

int readPrintBuffer(char* buf, int maxLength) {
	int i, initial_size = buf_size;
	for(i=0;i<maxLength-1;i++) {
		if(buf_size==0) break; //buffer is emtpy
		buf[i] = buffer[buf_read_index];
		buf_read_index = (buf_read_index+1)%100;
		buf_size--;
	}
	buf[i]='\0';
	return initial_size-buf_size;
}

void read_timer_fired(int id) {
	char buf[10];
	int length = readPrintBuffer(buf, 10);
	if(length !=0) printf(buf);
}

void print_timer_fired(int id) {
	buf_printf("print timer fired at %i %i\n", (int)getLocalMsTime());
}

void controller_init() {
	ase_printf("Module running %i\n",getHardwareID());
	printTimer = TimerManager_createPeriodicTimer(1000, 0, print_timer_fired);
	readTimer = TimerManager_createPeriodicTimer(500, 0, read_timer_fired);
}
