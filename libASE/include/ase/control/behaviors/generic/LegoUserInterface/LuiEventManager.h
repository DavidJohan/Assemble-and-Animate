#ifndef LUIEVENTMANAGER_H_
#define LUIEVENTMANAGER_H_
#include <stdbool.h>
bool LuiEventManager_isWaitingForEvent();
void LuiEventManager_waitForEvent();
signed char LuiEventManager_timeSinceEvent();
void LuiEventManager_init();

#endif /* LUIEVENTMANAGER_H_*/

