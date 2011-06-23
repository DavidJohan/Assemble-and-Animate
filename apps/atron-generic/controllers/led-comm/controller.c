#include <math.h>
#include <stdio.h>
#include <ase/Infrastructure.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/atron.h>
#include <ase/tools/Timer/TimerManager.h>

#define LED_MSG     128
Timer_t* timer;
void timer_fired(int id) {  //evoked every 200 ms
   char msg[4] = {LED_MSG, 0, 0, atronApi_getLeds()};
   sendMessageToAllNeighbors(msg, sizeof(msg));
}

void handle_msg(Msg_t* msg) {
   atronApi_setLeds(msg->message[3]+1);
}

void controller_init() {
   MsgManager_subscribe(LED_MSG, handle_msg);
   timer = TimerManager_createPeriodicTimer(
                           200, 0, timer_fired);
}
