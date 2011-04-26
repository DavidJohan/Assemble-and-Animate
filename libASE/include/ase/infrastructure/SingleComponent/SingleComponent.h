#include <stdbool.h>
void SingleComponent_init(int initialization_parameter, void (*init_fn)(int), int (*act_fn)(), bool (*done_fn)());
void SingleComponent_act();
void SingleComponent_handleMessage(char* message, char messageSize, char channel);
