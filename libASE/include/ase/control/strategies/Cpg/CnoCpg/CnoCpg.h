/**
\file CnoCpg.h

CnoCpg.h provides a simple interface to an implementation of as simple artificial central pattern generator.
This is a specific implementation on the abstract CPG implementation CpgSuper.h
The specific CPG is based on coupled non-linear oscillators, as described in:

	Marbach, D. and Ijspeert, A.J. (2005)
	Online Optimization of Modular Robot Locomotion.
	Proceedings of the IEEE Int. Conference on Mechatronics and Automation (ICMA 2005), pp. 248-253

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef CPG_H_
#include <ase/config/ConfigASE.h>
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * Struct extends CpgSuper_t with parameters specific to this CPG
 * \extends CpgSuper_t
 */
typedef struct {
	CpgSuper_t super;
	float E;
	float tau;
	float alpha;
	float S;
} CnoCpg_t;


/**
Initialize the class
*/
void CnoCpg_init();

/**
Initialize the central pattern generator
*/
void CnoCpg_initialize(CnoCpg_t* cpg);

/**
 * Performs one itteration (state update) of the CPG
 * is called automatically
 */
void CnoCpg_iterate(CnoCpg_t *cpg);

/**
 * Reset the state of the CPG to default/initial values
 */
void CnoCpg_resetState(CnoCpg_t *cpg);

/**
 * Set the number of updates per CPG period, default is 25
 */
void CnoCpg_setUpdateRate(float updatesPerPeriode, CnoCpg_t* cpg);

/**
 * Couple this CPG with another using given coupling parameters
 */
void CnoCpg_addCoupling(float x1, float v1, CpgSuperCoupling_t *coupling, CnoCpg_t *cpg);

#define CPG_H_

#endif /* CPG_H_ */

