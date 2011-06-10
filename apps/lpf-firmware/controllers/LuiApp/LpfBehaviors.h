
#ifndef LUIBEHAVIORS_H_
#define LUIBEHAVIORS_H_
#include <ase/control/arbitration/Subsumption/Subsumption.h>

void playback_start(void* data);
void playback_stop(void* data);
void playback_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

void knn_behavior_start(void* data);
void knn_behavior_stop(void* data);
void knn_behavior_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	int dir;
	long startTime;
} LpfBehavior_escape_t;
void escape_start(void* data);
void escape_stop(void* data);
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long lastSoundTime;
} LpfBehavior_move_t;
void move_start(void* data);
void move_stop(void* data);
void move_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long lastSoundTime;
	long delay;
	int lastIntensity;
} LpfBehavior_geiger_t;
void geiger_start(void* data);
void geiger_stop(void* data);
void geiger_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  char dummy;
} LpfBehavior_bird_song_t;
void bird_song_start(void* data);
void bird_song_stop(void* data);
void bird_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


typedef struct {
  char dummy;
} LpfBehavior_bee_song_t;
void bee_song_start(void* data);
void bee_song_stop(void* data);
void bee_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	char role[5];
	long lastChangeTime;
} LpfBehavior_dance_t;
void dance_start(void* data);
void dance_stop(void* data);
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  bool tilt_play;
} LpfBehavior_fly_t;
void fly_start(void* data);
void fly_stop(void* data);
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  char dummy;
} LpfBehavior_sea_t;
void sea_start(void* data);
void sea_stop(void* data);
void sea_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

#endif 



