#include <stdio.h>
#include <stdlib.h>
#include <ase/KeyController.h>
#include <ase/RotationController.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/CommDefs.h>
#include <ase/control/behaviors/generic/CpgGait/CpgGait.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/DynamixelApi.h>
#include <pthread.h>

void *listenForKeyInput() {
	while(1) {
		char command;
		printf("Type in a command: \n");
		printf("	p - pause for one period\n");
		printf("	s - stop\n");
		printf("	S - start\n");
		printf("	h - home all for one period\n");
		printf("	q - turn left (pause left for one period)\n");
		printf("	e - turn right (pause right for one period)\n");
		printf("	f - Increase frequency\n");
		printf("	g - Decrease frequency\n");
		printf("	h - Increase lift  modules phase-shift\n");
		printf("	j - Decrease lift modules phase-shift\n");
		printf("	k - Increase stride modules phase-shift\n");
		printf("	l - Decrease stride modules phase-shift\n");
		scanf("%c", &command);
		printf("The command you typed was %c\n", command);
		int i;
		if(command=='p') {
			for(i=0;i<getNumberOfActuators();i++) {
				RotationController_getRotation(i)->pause = true;
				RotationController_getRotation(i)->pauseStart = getLocalTime();
			}
		}
		if(command=='h') {
			for(i=0;i<getNumberOfActuators();i++) {
				dynamixelApi_setGoalPos(i,512);
			}
			for(i=0;i<getNumberOfActuators();i++) {
				RotationController_getRotation(i)->pause = true;
				RotationController_getRotation(i)->pauseStart = getLocalTime();
			}
		}
		if(command=='s') {
			for(i=0;i<getNumberOfActuators();i++) {
				RotationController_getRotation(i)->running = false;
			}
		}
		if(command=='S') {
			for(i=0;i<getNumberOfActuators();i++) {
				RotationController_getRotation(i)->running = true;
			}
		}

		//6,7,8,9
		if(command=='q') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==4||i==5||i==6||i==7) {
					RotationController_getRotation(i)->pause = true;
					RotationController_getRotation(i)->pauseStart = getLocalTime();
				}
			}
		}
		if(command=='e') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0||i==1||i==2||i==3) {
					RotationController_getRotation(i)->pause = true;
					RotationController_getRotation(i)->pauseStart = getLocalTime();
				}
			}
		}
		if(command=='f'||command=='g') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(command=='f') RotationController_getRotation(i)->frequency += 0.1f;
				if(command=='g') RotationController_getRotation(i)->frequency -= 0.1f;
			}
			printf("New frequency = %f\n",RotationController_getRotation(0)->frequency);
		}
		//stride modules: 4=>2,3=>1,8=>6,7=>5
		//lift modules: 2=>0,5=>3,6=>4,9=>7
		if(command=='h'||command=='j') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==0||i==3||i==4||i==7) {
					if(command=='h') RotationController_getRotation(i)->phaseDiff += 0.1f;
					if(command=='j') RotationController_getRotation(i)->phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for lift modules = %f\n",RotationController_getRotation(0)->phaseDiff);
		}
		if(command=='k'||command=='l') {
			for(i=0;i<getNumberOfActuators();i++) {
				if(i==2||i==1||i==6||i==5) {
					if(command=='k') RotationController_getRotation(i)->phaseDiff += 0.1f;
					if(command=='l') RotationController_getRotation(i)->phaseDiff -= 0.1f;
				}
			}
			printf("New phase-shift for stride modules = %f\n",RotationController_getRotation(0)->phaseDiff);
		}
	}
}
void KeyController_startKeyInputControl() {
	pthread_t listenThread;
	pthread_create(&listenThread, NULL, listenForKeyInput, NULL);
}
