#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiEventManager.h>

static signed char time_since_last_event=-1;
static signed char deviceReadList_old[10];
static signed char deviceRCList_old[10];
static signed char selectedBehaviorList_old[5];
static Timer_t* eventTimer;

bool updateInputs() {
	bool event = false;
	for(int i=0;i<LuiManager_getNumberOfInputDevices();i++) {
	  if(LuiManager_getDeviceReadList()[i]!=deviceReadList_old[i]) event = true;
	  deviceReadList_old[i] = LuiManager_getDeviceReadList()[i];
	}
	return event;
}
bool updateRC() {
	bool event = false;
	for(int i=0;i<LuiManager_getNumberOfOutputDevices();i++) {
		if(LuiManager_getDeviceRCList()[i]!=deviceRCList_old[i]) event = true;
		deviceRCList_old[i] = LuiManager_getDeviceRCList()[i];
	}
	return event;
}
bool updateBehaviors() {
	bool event = false;
	for(int i=0;i<5;i++) {
		if(LuiManager_getSelectedBehaviorList()[i]!=selectedBehaviorList_old[i]) event = true;
		selectedBehaviorList_old[i] = LuiManager_getSelectedBehaviorList()[i];
	}
	return event;
}
static void checkForEvents() {
  	bool event = false;
	if(LuiEventManager_isWaitingForEvent()) {
		event |=updateRC();
		event |=updateBehaviors();
		if(event) {
			ase_printf("#play escape.wav 50 1\n");
			time_since_last_event = 0;
		}
	}
	else {
		event |= updateInputs();
		if(event) {
			time_since_last_event = 0;
			ase_printf("EVENT!:::\n");
			for(int i=0;i<LuiManager_getNumberOfInputDevices();i++) {
				ase_printf("%i, ",LuiManager_getDeviceReadList()[i]);
			}
			ase_printf("\n");
		}
		else {
			time_since_last_event++;
		}
	}
	if(time_since_last_event>100) {
		time_since_last_event=100;
	}
}

bool LuiEventManager_isWaitingForEvent() {
  return time_since_last_event==-1;
}

void LuiEventManager_waitForEvent() {
	updateRC(); //remove pending events
	updateBehaviors(); //remove pending events
	time_since_last_event=-1; //wait for event
}

signed char LuiEventManager_timeSinceEvent() {
	return time_since_last_event;
}

static void event_timer_fired(int id) {
	LuiManager_updateDeviceReadList();
	if(!LuiManager_deviceReadSuccess()) {
		ase_printf("---READ ERROR---\n");
		return;
	}
	checkForEvents();	
}

void LuiEventManager_init() {
	eventTimer = TimerManager_createPeriodicTimer(300, 1, event_timer_fired);
}
