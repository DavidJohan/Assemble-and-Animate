#ifndef BEATDETECTOR_H_
#define BEATDETECTOR_H_
#include <stdbool.h>
void BeatDetector_init();
void BeatDetector_reset();
long BeatDetector_lastBeatTime();
long BeatDetector_nextBeatTime();
long BeatDetector_getBeatPeriod();
bool BeatDetector_gotBeat();
void BeatDetector_clearBeat();

#endif
