#include <ase/targets/AbstractModuleApi.h>
#include <ase/tools/Timer/TimerManager.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/targets/dynamixel.h>
#include <ase/control/strategies/kNN/kNN.h>
#include <ase/control/strategies/Playback/Playback.h>
#include <ase/control/arbitration/Subsumption/Subsumption.h>

//#include "LuiManager.h"
//#include "LuiBoard.h"
//#include "LuiEventManager.h"
//#include "LuiBehaviorManager.h"
//#include "LpfBehaviors.h"
//#include "LuiTraining.h"
#include <ase/control/behaviors/generic/LegoUserInterface/LegoUserInterface.h>

static kNN_t kNN_behavior;
static kNN_t kNN_compound;
static Playback_t playback_data;

static LpfBehavior_fly_t fly_data;
static LpfBehavior_dance_t dance_data;
static LpfBehavior_bee_song_t bee_data;
static LpfBehavior_bird_song_t bird_data;
static LpfBehavior_geiger_t geiger_data;
static LpfBehavior_move_t move_data;
static LpfBehavior_escape_t escape_data;

void installBehaviors(Subsumption_t* subsumption) {
	LuiBehaviorManager_addBehavior(1, &escape_data, escape_start, escape_act, escape_stop,'s', subsumption);
	LuiBehaviorManager_addBehavior(2, &move_data, move_start, move_act, move_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(3, &geiger_data, geiger_start, geiger_act, geiger_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(4, &bird_data, bird_song_start, bird_song_act, bird_song_stop,  's', subsumption);
	LuiBehaviorManager_addBehavior(5, &bee_data, bee_song_start, bee_song_act, bee_song_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(6, &fly_data, fly_start, fly_act, fly_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(7, &dance_data, dance_start, dance_act, dance_stop, 's', subsumption);
	LuiBehaviorManager_addBehavior(97,&kNN_compound, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 'c', subsumption);
	LuiBehaviorManager_addBehavior(98,&playback_data, playback_start, playback_act, playback_stop, 'r', subsumption);
	LuiBehaviorManager_addBehavior(99,&kNN_behavior, knn_behavior_start, knn_behavior_act, knn_behavior_stop, 't', subsumption);
}

void controller_init() {
  LPFApi_init();
  LUI_init();
  kNN_init(&kNN_behavior, 1);
  kNN_init(&kNN_compound, 1);
  Playback_init(&playback_data);
  installBehaviors(LUI_getSubsumptionProcess());
}
