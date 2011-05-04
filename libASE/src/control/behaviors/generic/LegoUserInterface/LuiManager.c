#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/non-public/lpf.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiManager.h>

static signed char selectedBehaviorList[5];
static signed char deviceCountList[7];
static signed char deviceRCList[100];
static signed char deviceReadList[100];
static int nDevices;
static bool readSuccess;

void LuiManager_init() {
	int i;
	for(i=0;i<100;i++) deviceRCList[i] = 0;
	for(i=0;i<7;i++) deviceCountList[i] = 0;
	for(i=0;i<100;i++) deviceReadList[i] = 0;
	nDevices=0;
}

//skal være en call-back
void LuiManager_applyControlOutput(signed char* outputValues, char nOutput) { 
	int mCount  = LPFApi_getNumberOfMotors();
	int rgbCount  = LPFApi_getNumberOfRGBLights();
	int dispCount  = LPFApi_getNumberOfDisplays();
	if(nOutput==(mCount+rgbCount+dispCount)) {
		int i,index=0;
		for(i=0;i<mCount;i++) {
			signed char val = outputValues[index];
			LPFApi_setMotorPower(i, val);
			index++;
		}
		for(i=0;i<rgbCount;i++) {
			signed char val = outputValues[index];
			if(val<0) val=-val;
			LPFApi_setRGBLight(i, val, val, val);
			index++;
		}
		for(i=0;i<dispCount;i++) {
			index++;
			//what to do here?
			//LPFApi_printOnDisplay(0, 10, 10, 'X');
		}

	}
	else {
		ase_printf("Warning: Number of devices does not match length of output array\n");
	}
}


void LuiManager_setDeviceRCList(signed char* data, int nData, int offset) {
	int i;
	for(i=0;i<100;i++) { 
		if(i<nData) deviceRCList[i] = data[i]+offset;
		else deviceRCList[i] = 0;
	}
}

void LuiManager_setSelectedBehaviorList(signed char* behaviors) {
	int i;
	for(i=0;i<5;i++) { 
		selectedBehaviorList[i] = behaviors[i];
	}
}

//skal være en call-back
int LuiManager_updateDeviceReadList() {
	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	readSuccess = true;
	signed char result[3];
	int i,index=0;
	for(i=0;i<dsCount;i++) {
		bool res = LPFApi_getDetectSensor(i, result);
		deviceReadList[index] = (res)?result[0]:255;
		if(!res) readSuccess = false;
		index++;
	}

	for(i=0;i<lsCount;i++) {
		bool res = LPFApi_getLightSensor(i, result);
		deviceReadList[index] = (res)?result[0]:255;
		if(!res) readSuccess = false;
		index++;
	}

	for(i=0;i<tsCount;i++) {
		bool res = LPFApi_getTiltSensor(i, result);
		deviceReadList[index] = (res)?result[0]:255;
		if(!res) readSuccess = false;
		index++;
	}

	for(i=0;i<msCount;i++) {
		bool res = LPFApi_getTiltMemsSensor(i, result);
		deviceReadList[index] = (res)?result[0]:255;
		deviceReadList[index+1] = (res)?result[1]:255;
		deviceReadList[index+2] = (res)?result[2]:255;
		if(!res) readSuccess = false;
		index+=3;
	}
	return index;
}

bool LuiManager_deviceReadSuccess() {
	return readSuccess;
}

//skal være en call-back
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
}


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

//skal være en call-back
int LuiManager_getNumberOfInputDevices() {
  	int dsCount = LPFApi_getNumberOfDetectSensors();
	int lsCount = LPFApi_getNumberOfLightSensors();
	int tsCount = LPFApi_getNumberOfTiltSensors();
	int msCount = LPFApi_getNumberOfTiltMemsSensors();
	return dsCount+lsCount+tsCount+msCount;
}

//skal være en call-back
int LuiManager_getNumberOfOutputDevices() {
	int mCount  = LPFApi_getNumberOfMotors();
	int rgbCount  = LPFApi_getNumberOfRGBLights();
	int dispCount  = LPFApi_getNumberOfDisplays();
	return mCount+rgbCount+dispCount;
}
