#include <SingleComponent.h>
#include <ase/targets/atron/AtronApi.h>
#include <ase/robodays_sculpture.h>

void controller_init() {
  SingleComponent_init(atronApi_getHardwareID(),&robodays_sculpture_init,&robodays_sculpture_act,&robodays_sculpture_isDone);
}

void controller_act()  {
  SingleComponent_act();
}

void handleMessage(char* message, char messageSize, char channel) {
  SingleComponent_handleMessage(message,messageSize,channel);
}
