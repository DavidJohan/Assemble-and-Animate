/**
 * \file CpgGait.h
 *
 * CpgGait.h provides a generic, distributed, locomotion system for any actuated modular robot.
 * The locomotion system is based in Hopf central pattern generators. It uses one
 * CPG per module as a shared global clock (cpg's are in phase) and one locally coupled
 * CPG per actuator. The local cpg's are only coupled to the global clock cpg and
 * their phase shift is relative to this global clock.
 *
 *
 * \see HopfCartesianCpg.h
 * \todo Move all references to log files elsewhere
 *
 *  Created on: May 4, 2009
 *  \author David Johan Christensen
 */


#ifndef CPGGAIT_H_
#include <stdbool.h>
#include <stdio.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpgManager.h>
#define CPGGAIT_N_FEEDBACK_TERMS	4
/**
 * Initialize the gait with default values and starts to update the Hopf cpg's,
 * couples the sensor feedback from encoders and updates the actuators
 * position based on the output of the local cpg's.
 */
void CpgGait_init();

//TODO change to strategy_t
void CpgGait_start();
void CpgGait_stop();
bool CpgGait_isStarted();
HCCpg_t* CpgGait_getClockCpg();
HCCpg_t* CpgGait_getLocalCpg(int index);
/**
 * Set the three different couplings strengths (global to global, global to local and local to local)
 *
 */
void CpgGait_setCouplingStrenghts(float g2gStrength, float g2lStrength, float l2gStrength);

/**
 * Set the amplitude of the CPG which controls actuator index [0, 1, 2,..]
 */
void CpgGait_setAmplitude(float amplitude, int index);

/**
 * Set the offset of the CPG which controls actuator index [0, 1, 2,..]
 */
void CpgGait_setOffset(float offset, int index);

/**
 * Set the offset of the CPG which controls actuator index [0, 1, 2,..]
 */
void CpgGait_setPhaseShift(float phaseShift, int index);

float CpgGait_getAmplitude(int index);
float CpgGait_getOffset(int index);
float CpgGait_getPhaseShift(int index);
float CpgGait_getFrequency(int index);
float CpgGait_getFeedbackTerm(int nr, int index);


/**
 * Set the frequency (in hz) of the CPG which controls actuator index [0, 1, 2,..]
 */
void CpgGait_setFrequency(float hz, int index);
void CpgGait_setGlobalFrequency(float hz);

/**
 * Set the feedback terms using in Hopf cpg's, nr indicates which feedback type:
 * 0 - Current actuator pos added to the X equation of the Hopf CPG
 * 1 - Current actuator error added to the X equation of the Hopf CPG
 * 2 - Current actuator pos added to the Y equation of the Hopf CPG
 * 3 - Current actuator error added to the Y equation of the Hopf CPG
 */
void CpgGait_setFeedbackTerm(float term, int nr, int actuatorIndex);

/**
 * Set a call back function to handle the actuation based on the cpg instead of
 * the default setActuatorPosition(goal,actuator)
 */
void CpgGait_setActuatorPosHandler( void (handler)(HCCpg_t*, int));

/**
 * Get current controller as an array of floats
 * Format: ampl0, phase0,freq0,offset0, ampl1, phase1, freq1, offset1, ...
 * Followed by: feedbackTerms0, feedbackTerms1,...
 */
void CpgGait_getController(float* controller);

/**
 * Set current controller as an array of floats
 * Format: ampl0, phase0,freq0,offset0, ampl1, phase1, freq1, offset1, ...
 * Followed by: feedbackTerms0, feedbackTerms1,...
 */
void CpgGait_setController(float* controller);

/**
 * Set if encoder feedback should be added to the local Hopf Cpg's
 * otherwise the feedback terms will be ignored
 */
void CpgGait_setUseFeedback(int feedback);

/**
 * Depricated Method
 */
void CpgGait_logController(int id, FILE* file); //depricated

/**
 * Depricated Method
 */
void CpgGait_loadController(int id, FILE* file); //depricated

/**
 *
 */
//void CpgGait_loadAverageController(int id1, int id2, FILE* file);
void CpgGait_loadAverageController(int id, int nControllers, FILE* file);


/**
 * Depricated Method
 */
void CpgGait_setLoggers(FILE* lCPG, FILE* lControl); //depricated


/**
 * Depricated Method
 */
void CpgGait_logCpg(FILE* log); //depricated

#define CPGGAIT_H_

#endif /* CPGGAIT_H_ */
