/**
 * \file Playback.h
 * This file provides an playback controller
 *
 * \date December 2010
 * \author David Johan Christensen, Center for Playware, Technical University of Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef PLAYBACK_H_
#include <ase/config/ConfigASE.h>
#define MAX_PLAYBACK_SETS		100
#define MAX_PLAYBACK_INPUTS		4
#define MAX_PLAYBACK_OUTPUTS	4

typedef struct {
	signed char output[MAX_PLAYBACK_OUTPUTS];
	long timeMs;
} PlaybackSet_t;


typedef struct {
	short label;
	PlaybackSet_t sets[MAX_PLAYBACK_SETS]; //better data structure nessecary
	int nSets;
	int nextSet;
	int lastPlaybackIndex;
	long startTimeMs;
} Playback_t;

/**
 * Initialize the playback controller with default parameters
 */
void Playback_init(Playback_t* process);

/**
 */
void Playback_getOutput(Playback_t* process, signed char* output, int nOutputs);


/**
 */
bool Playback_record(Playback_t* process, signed char* output, int nOutputs);


void Playback_startPlayback(Playback_t* process);
void Playback_startRecording(Playback_t* process);
void Playback_clearRecordings(Playback_t* process);


#define PLAYBACK_H_

#endif
