#include <stdio.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/control/strategies/Gradient/GradientManager.h>
#include <ase/communication/CommDefs.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>


static Gradient_t* gradient0;
static Gradient_t* gradient1;
static int id;
static void gradient_updated(char* topic, Event_t* event)  {
	if(event->val_prt==gradient0||event->val_prt==gradient1) {
		ase_printf("{%i, %i, %i},\n", id, Gradient_getValue(gradient0), Gradient_getValue(gradient1));
	}
}

void controller_init() {
	id = getHardwareID();
	ase_printf("Running [ ID = %i ]\n",id);
	EventManager_subscribe(GRADIENT_UPDATED_EVENT, gradient_updated);
	gradient0 = GradientManager_createGradient(0, 1.0f);
	gradient1 = GradientManager_createGradient(1, 10.0f);

	if(getHardwareID()==0) {
		Gradient_setSeed(gradient0, true, 5);
	}
	if(getHardwareID()==6) {
		Gradient_setSeed(gradient1, true, 10);
	}

	Strategy_start((Strategy_t*)gradient0, NULL);
	Strategy_start((Strategy_t*)gradient1, NULL);
}
