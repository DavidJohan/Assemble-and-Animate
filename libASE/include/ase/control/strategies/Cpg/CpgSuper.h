/**
\file CpgSuper.h
Generic, super class for CPG implementation. Not to be instantiated directly, but is used by sub-classes.
Known sub-classes: CnoCpg.h and HopfCartesianCpg.h
\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef CPGSUPER_H_
#include <ase/config/ConfigASE.h>
#include <ase/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.h>
#include <ase/communication/Message.h>
#include <stdint.h>
#include <stdbool.h>
#define CPG_UPDATED_EVENT "CPG_UPDATED_EVENT"

/**
 * Generic struct that contains the common data of a CPG
 * \extends PeriodicStrategy_t
 */
typedef struct {
	PeriodicStrategy_t super;
	//short label; //\todo consider move 'label' to strategy?
	float ampl;
	float x;
	float y;
	float x_offset;
	float y_offset;
	int nCouplings;
	int updatesPerperiode;
} CpgSuper_t;

/**
 * Generic Struct that contains the data of a coupling between to CPGs
 */
typedef struct {
	//Entity_t from;
	//Entity_t to;
	short from_label;
	short to_label;
	uint8_t apply_to_channels[MAX_NODE_DEGREE]; // \todo implement a bit array to save memory
	float phaseDiff;
	float strength;
} CpgSuperCoupling_t;

/**
 * Generic struct used as a communication message contains the state of
 * a CPG, used for coupling CPGs over communication channels.
 * \extends Message_t
 */
typedef struct {
	Message_t super;
	float x;
	float y;
} CpgSuperMsg_t;


/**
 * Set the state (x,y) of the CPG
 */
void CpgSuper_setState(CpgSuper_t *cpg, float new_x, float new_y);

/**
 * Set the offset of the CPG
 */
void CpgSuper_setOffset(CpgSuper_t *cpg, float xOffset, float yOffset);

/**
 * Get the offset on the 'x' component
 */
float CpgSuper_getOffsetX(CpgSuper_t *cpg);

/**
 * Get the offset on the 'y' component
 */
float CpgSuper_getOffsetY(CpgSuper_t *cpg);


/**
 * Get the amplitude of the CPG
 */
float CpgSuper_getAmplitude(CpgSuper_t *cpg);

/**
 * Set the amplitude of the CPG
 */
void CpgSuper_setAmplitude(CpgSuper_t *cpg, float amplitude);

/**
 * Get the 'Y' state variable:
 * Y = amplitude*y+yOffset
 */
float CpgSuper_getY(CpgSuper_t *cpg);

/**
 * Get the 'X' state variable:
 * X = amplitude*x+xOffset
 */
float CpgSuper_getX(CpgSuper_t *cpg);

/**
 *Send the state of the CPG to all neighbors
 */
void CpgSuper_sendState(CpgSuper_t* cpg, CpgSuperMsg_t* msg);

/**
 * Reset the generic part of the CPG state
 */
void CpgSuper_resetState(CpgSuper_t *cpg);

/**
 * Get the frequency of the CPG in oscillations per second.
 */
float CpgSuper_getFrequency(CpgSuper_t* cpg);

/**
 * Set frequency of the CPG in oscillations per second.
 */
void CpgSuper_setFrequency(float hz, CpgSuper_t* cpg);

/**
 * Initialize the CPG with default parameters
 */
void CpgSuper_initialize(CpgSuper_t *cpg, uint8_t label, float hz);

/**
 * Update the CPG if it is time to do so, will also send the state to neighbors
 */
bool CpgSuper_updateIfTime(CpgSuper_t* cpg, void (*iterate)(CpgSuper_t*));

/**
 * Set if this coupling should accept (apply) coupling messages from specific channels.
 * Function can be used to make a CPG affected by a local CPG only and not by CPG (using the same id)
 * on neighbour modules
 */
void CpgSuper_setApplyToChannel(CpgSuperCoupling_t* coupling, int channel, int applyTo);

/**
 * Set the coupling strength parameter
 */
void CpgSuper_setStrength(CpgSuperCoupling_t* coupling, float strength);

/**
 * Get the coupling strength
 */
float CpgSuper_getStrength(CpgSuperCoupling_t* coupling);

/**
 * Set the phase difference of a coupling in radians
 */
void CpgSuper_setPhaseDifference(CpgSuperCoupling_t* coupling, float phaseDifference);

/**
 * Get the phase difference in radians
 */
float CpgSuper_getPhaseDifference(CpgSuperCoupling_t* coupling);



#define CPGSUPER_H_

#endif /* CPGSUPER_H_ */

