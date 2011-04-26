/**
 * \file HopfCartesianCpgManager.h
 * HopfCartesianCpgManager.h manages a number of Hopf CPGs. Takes care of timing,
 * memory management, parsing of messages, couplings, etc.
 *
 * \author David Johan Christensen, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef HOPFCARTESIANCPGMANAGER_H_
#define HOPFCARTESIANCPGMANAGER_H_
#include <ase/control/strategies/Cpg/HopfCartesianCpg/HopfCartesianCpg.h>

/**
 * Create a Hopf CPG with default parameters.
 * hz specify the frequency of the created CPG
 * The label specify the
 */
HCCpg_t* HCCpgManager_createCpg(short label, float hz);

/**
 * Creates a coupling from cpgs with label "fromLabel" to "toLabel"
 * with a coupling strength and desired phase difference.
 */
CpgSuperCoupling_t* HCCpgManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff);


#endif /* HOPFCARTESIANCPGMANAGER_H_ */
