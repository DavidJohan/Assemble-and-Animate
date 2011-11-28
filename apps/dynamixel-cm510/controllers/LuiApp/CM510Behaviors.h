
#ifndef CM510BEHAVIORS_H_
#define CM510BEHAVIORS_H_
#include <ase/control/arbitration/Subsumption/Subsumption.h>
#include <stdbool.h>
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
	long detectTime;
	bool playingSound;
} CM510Behavior_seek_t;
void seek_start(void* data);
void seek_stop(void* data);
void seek_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

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
  char dummy;
} CM510Behavior_monkey_song_t;
void monkey_song_start(void* data);
void monkey_song_stop(void* data);
void monkey_song_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);



typedef struct {
	bool active;
	bool cycle;
	bool phase;
	int range;
	long periodTime;
	long periodStartTime;
} CM510Behavior_dance_dof_t;

typedef struct {
	bool started;
	CM510Behavior_dance_dof_t dofs[25];
}CM510Behavior_dance_t;

void dance_start(void* data);
void dance_stop(void* data);
void dance_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
  bool tilt_play;
} CM510Behavior_fly_t;
void fly_start(void* data);
void fly_stop(void* data);
void fly_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
	bool ended;
	bool started;
}CM510Behavior_play_dead_t;
void play_dead_start(void* data);
void play_dead_stop(void* data);
void play_dead_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
}CM510Behavior_follow_t;
void follow_start(void* data);
void follow_stop(void* data);
void follow_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


typedef struct {
	long startTime;
	long itteration;
	bool playingAngrySound;
}CM510Behavior_attack_t;
void attack_start(void* data);
void attack_stop(void* data);
void attack_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	bool playingSound;
}CM510Behavior_hide_t;
void hide_start(void* data);
void hide_stop(void* data);
void hide_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);

typedef struct {
	long startTime;
	bool actSurprised;
	char randomDirs[20];
}CM510Behavior_play_t;
void play_start(void* data);
void play_stop(void* data);
void play_act(signed char* input, char nInputs, signed char* output, char nOutputs, void* data);


#endif 



