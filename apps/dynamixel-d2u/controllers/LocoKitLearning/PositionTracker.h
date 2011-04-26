#include <stdbool.h>

void PositionTracker_start(int port, char* host);
int PositionTracker_getCurrentPos();
float PositionTracker_getLastUpdateTime();
bool PositionTracker_isReady();
bool PositionTracker_isPaused();
