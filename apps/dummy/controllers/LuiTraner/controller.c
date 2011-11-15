#include <stdio.h>
#include <stdlib.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <ase/control/controller.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>
#include <ase/control/behaviors/generic/LegoUserInterface/LuiBehaviorManager.h>
#include "CM510Behaviors.h"
#include "BeatDetector.h"
static kNN_t* kNN_behavior;
static kNN_t* kNN_compound;
static Playback_t* playback_data;

void getData() {
	ase_printf("Hello world\n")
}
void controller_init() {
  kNN_behavior = malloc(sizeof(kNN_t));
  playback_data = malloc(sizeof(Playback_t));
  kNN_init(kNN_behavior, 1);
  Playback_init(playback_data);
  getData();

}
