#include <stdio.h>
#include <stdlib.h>

#include <ase/infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/communication/Message.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/control/controller.h>
#include "controller.h"

void delay_ms(long delay_ms) {
	long startTime = dynamixelApi_getMsTime();
	while((startTime+delay_ms)>dynamixelApi_getMsTime());
}

void aliveTimer(int id) {
	ase_printf("alive...\n");
	dynamixelApi_CM510_toggleLed(0);
}

void controller_init() {
  dynamixelApi_CM510_init();
  dynamixelApi_setup(1, NULL); 	delay_ms(250);
  dynamixelApi_connect(10); 	delay_ms(150); //0
  dynamixelApi_connect(3); 		delay_ms(150); //1
  dynamixelApi_connect(4); 		delay_ms(150); //2
  TimerManager_createPeriodicTimer(1000, 0, aliveTimer); //reads behavior brick directly
}
