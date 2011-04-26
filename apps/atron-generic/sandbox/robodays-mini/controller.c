#include <SingleComponent.h>
#include <stdio.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/robodays_mini.h>
#include <ase/ModuleStats.h>

unsigned char ittCount=0;
void controller_init() {
  printf("Running [ ID = %i ]\n",atronApi_getHardwareID());
  SingleComponent_init(atronApi_getHardwareID(),&robodays_mini_init,&robodays_mini_act,&robodays_mini_isDone);
}

void controller_act()  {
  SingleComponent_act();
  if(ittCount%500==0) {
	  ModuleStats_print();
  }
  ittCount++;
}


void handleMessage(char* message, char messageSize, char channel) {
	ModuleStats_gotMessage(message, messageSize, channel);
	SingleComponent_handleMessage(message,messageSize,channel);
}
