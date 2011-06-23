#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/tools/Timer/TimerManager.h>

Gradient_t *g0;
static void handler(char* topic, Event_t* event)  {
	Gradient_t* g = (Gradient_t*)event->val_prt;
	ase_printf("Gradient = %i\n", Gradient_getValue(g));
}

void controller_init() {
	EventManager_subscribe(GRADIENT_UPDATED_EVENT, handler);
	g0 = GradientManager_createGradient(0, 1.0f);
	GradientManager_startGradient(g0);
	if(getHardwareID()==3) {
		Gradient_setSeed(g0, true, 5);
	}
}
