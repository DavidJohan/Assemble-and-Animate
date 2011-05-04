
#ifndef CM510BEHAVIORS_H_
#define CM510BEHAVIORS_H_
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
} CM510Behavior_escape_t;
void escape_start(void* data);
void escape_stop(void* data);
void escape_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long lastSoundTime;
} CM510Behavior_move_t;
void move_start(void* data);
void move_stop(void* data);
void move_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long lastSoundTime;
	long delay;
	int lastIntensity;
} CM510Behavior_geiger_t;
void geiger_start(void* data);
void geiger_stop(void* data);
void geiger_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  char dummy;
} CM510Behavior_bird_song_t;
void bird_song_start(void* data);
void bird_song_stop(void* data);
void bird_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


typedef struct {
  char dummy;
} CM510Behavior_bee_song_t;
void bee_song_start(void* data);
void bee_song_stop(void* data);
void bee_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	char role[5];
	long lastChangeTime;
} CM510Behavior_dance_t;
void dance_start(void* data);
void dance_stop(void* data);
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  bool tilt_play;
} CM510Behavior_fly_t;
void fly_start(void* data);
void fly_stop(void* data);
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

#endif 



