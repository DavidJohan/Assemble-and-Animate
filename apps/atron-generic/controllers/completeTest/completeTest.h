#include <stdbool.h>
#include <ase/infrastructure/EventManager/EventManager.h>

#define COMPLETETEST_DONE_EVENT "COMPLETETEST_DONE_EVENT"

void completeTest_init(int self_id);
void completeTest_act(char* topic, Event_t* event);
