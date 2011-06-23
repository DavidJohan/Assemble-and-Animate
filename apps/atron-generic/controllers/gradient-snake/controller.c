#include <stdio.h>
#include <math.h>
#include <ase/Infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/communication/SharedTimer/SharedTimer.h>
#include <ase/communication/Gossip/GossipManager.h>

void ctrl_act(char* topic, Event_t* event) {
  float pos = sin(getLocalTime()+1)/4.0f;
  setActuatorPosition(pos, 0);
}

void controller_init() {
  EventManager_subscribe(ACT_EVENT, ctrl_act);
}


