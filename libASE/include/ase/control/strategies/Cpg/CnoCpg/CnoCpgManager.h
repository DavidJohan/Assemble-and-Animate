/**
\file CnoCpgManager.h

CnoCpgManager.h provides management of 'cno' artificial central pattern generators.


Uses: CpgSuperManager.h and CpgSuper.h

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef CPGMANAGER_H_
#include <ase/control/strategies/Cpg/CnoCpg/CnoCpg.h>

/**
 * Create a CPG with specified label, frequency and otherwise default parameters
 */
CnoCpg_t* CnoCpgManager_createCpg(short label, float hz);

/**
 * Create a CPG coupling from 'fromLabel' to 'toLabel' with a specified coupling strength and phase difference.
 */
CpgSuperCoupling_t* CnoCpgManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff);


#define CPGMANAGER_H_

#endif /* CPGMANAGER_H_ */


