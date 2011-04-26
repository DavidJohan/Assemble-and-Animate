/**
\file HopfCartesianCpg.h

HopfCartesianCpg.h provides a simple interface to an implementation of Hopf Central Pattern Generator (CPG) in polar coordinates.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef HOPFCARTESIANCPG_H_
#define HOPFCARTESIANCPG_H_

#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Struct that contains the state and parameters for a Hopf CPG
 * \extends CpgSuper_t
 */
typedef struct {
	CpgSuper_t parent;
	float h;
	float my;
	float gamma;
	float S;
	float omega;
	float omega_add;
	float x_add;
	float y_add;
	float omegaEffective;
	float encoderCoupling;
} HCCpg_t;

/**
 * Initialize the Component
 */
void HCCpg_init();

/**
 * Initialize the Hopf central pattern generator
 * with default parameters
 */
void HCCpg_initialize(HCCpg_t *cpg);

/**
 * Performs one itteration (Runge-Kutta based) of the CPG
 * which will update the state of the CPG
 */
void HCCpg_iterate(HCCpg_t *cpg);

/**
 * Set state to x=1 and y=0 and
 * clears all couplings
 */
void HCCpg_resetState(HCCpg_t *cpg);

/**
 * Set the number of times HCCpg_iterate(.) will be called in one
 * CPG period.
 */
void HCCpg_setUpdateRate(float updatesPerPeriode, HCCpg_t *cpg);

/**
 * Add coupling to cpg from another CPG, x1 and y1 are the state variables of the other CPG,
 * coupling specifies the coupling strength and desired phase difference
 */
void HCCpg_addCoupling(float x1, float y1, CpgSuperCoupling_t *coupling, HCCpg_t *cpg);

/**
 * Add a coupling directly to the X equation (e.g. feedback from a sensor)
 */
void HCCpg_coupleDirectlyX(float signal, HCCpg_t *cpg);

/**
 * Add a coupling directly to the Y equation (e.g. feedback from a sensor)
 */
void HCCpg_coupleDirectlyY(float signal, HCCpg_t *cpg);

#endif /* HOPFCARTESIANCPG_H_ */

