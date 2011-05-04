#ifndef LUIMANAGER_H_
#define LUIMANAGER_H_
void LuiManager_init();
void LuiManager_setDeviceRCList(signed char* data, int nData, int offset);
void LuiManager_setSelectedBehaviorList(signed char* behaviors);
signed char* LuiManager_getDeviceRCList();
void LuiManager_applyControlOutput(signed char* outputValues, char nOutput);
int LuiManager_updateDeviceCountList();
int LuiManager_updateDeviceReadList();
bool LuiManager_deviceReadSuccess();
signed char* LuiManager_getDeviceRCList();
signed char* LuiManager_getDeviceReadList();
signed char* LuiManager_getDeviceCountList();
signed char* LuiManager_getSelectedBehaviorList();
int LuiManager_getNumberOfDevices();
int LuiManager_getNumberOfInputDevices();
int LuiManager_getNumberOfOutputDevices();
#endif 
