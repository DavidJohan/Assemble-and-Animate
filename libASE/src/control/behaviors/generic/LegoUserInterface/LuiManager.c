#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>

static signed char selectedBehaviorList[5];
static signed char deviceCountList[7];
static signed char deviceRCList[100];
static signed char deviceReadList[100];
static int nDevices;
static bool readSuccess;
static long deviceRCListUpdateTime=-1000;

void LuiManager_init() {
	int i;
	for(i=0;i<100;i++) deviceRCList[i] = 0;
	for(i=0;i<7;i++) deviceCountList[i] = 0;
	for(i=0;i<100;i++) deviceReadList[i] = 0;
	nDevices=0;
}

void LuiManager_setDeviceRCList(signed char* data, int nData, int offset) {
	int i;
	for(i=0;i<100;i++) { 
		if(i<nData) deviceRCList[i] = data[i]+offset;
		else deviceRCList[i] = 0;
	}
	//ase_printf("%i %i\n",deviceRCList[0],deviceRCList[1]);
	deviceRCListUpdateTime = getLocalMsTime();
}

long LuiManager_getDeviceRCListUpdateTime() {
	return deviceRCListUpdateTime;
}

void LuiManager_setSelectedBehaviorList(signed char* behaviors) {
	int i;
	for(i=0;i<5;i++) { 
		selectedBehaviorList[i] = behaviors[i];
	}
}

//skal være en call-back
void LuiManager_applyControlOutput(signed char* outputValues, char nOutput) {
	applyControlOutput(outputValues, nOutput);
}
//skal være en call-back
int LuiManager_updateDeviceReadList() {
	return getControlInput(deviceReadList,100,&readSuccess);
}

bool LuiManager_deviceReadSuccess() {
	return readSuccess;
}

/*//skal være en call-back
int LuiManager_updateDeviceCountList() {
	deviceCountList[0] = LPFApi_getNumberOfMotors();
	deviceCountList[1] = LPFApi_getNumberOfDetectSensors();
	deviceCountList[2] = LPFApi_getNumberOfLightSensors();
	deviceCountList[3] = LPFApi_getNumberOfTiltSensors();
	deviceCountList[4] = LPFApi_getNumberOfTiltMemsSensors();
	deviceCountList[5] = LPFApi_getNumberOfRGBLights();
	deviceCountList[6] = LPFApi_getNumberOfDisplays();
	int i;
	for(i=0;i<7;i++) {
		nDevices+=deviceCountList[i];
	}
	return 7;
}*/


signed char* LuiManager_getDeviceRCList() {
	return deviceRCList;
}

signed char* LuiManager_getDeviceReadList() {
	return deviceReadList;
}

signed char* LuiManager_getDeviceCountList() {
	return deviceCountList;
}

signed char* LuiManager_getSelectedBehaviorList() {
  	return selectedBehaviorList;
}

int LuiManager_getNumberOfDevices() {
	return nDevices;
}

int LuiManager_getNumberOfInputDevices() {
	return getNumberOfInputs();
}

int LuiManager_getNumberOfOutputDevices() {
	return getNumberOfOutputs();
}
