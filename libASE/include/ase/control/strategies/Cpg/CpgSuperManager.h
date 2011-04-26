/**
 * \file CpgSuperManager.h
 * CpgSuperManager.h is a super class of specific cpgs.
 *
 * This class should never bee used directly unless from an class which
 * inherits from it (another sub cpg manager).
 *
 * \author David Johan Christensen, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef CPGSUPERMANAGER_H_
#define CPGSUPERMANAGER_H_
#include <ase/control/strategies/Cpg/CpgSuper.h>
#include <ase/tools/LinkedList/LinkedList.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>

/**
 * A manager of CPGs and couplings
 *
 *\extends Manager_t
 */
typedef struct {
	Manager_t super;
	listnode_t* cpgs;
	listnode_t* couplings;
	CpgSuperMsg_t msg;
	char sizeOfCpg;
	char sizeOfCoupling;
	void (*addCoupling)(float, float,CpgSuperCoupling_t*,CpgSuper_t*);
	void (*iterate)(CpgSuper_t*);
} CpgSuperManager_t;

/**
 * Create a "super" or "parent" CPG with default parameters.
 * hz specify the frequency of the created CPG
 * The label is used to identify the cpg
 */
CpgSuper_t* _CpgSuperManager_createCpg(short label, float hz, CpgSuperManager_t* manager);

/**
 * Creates a coupling from cpgs with label "fromLabel" to "toLabel"
 * with a coupling strength and desired phase difference.
 */
CpgSuperCoupling_t* _CpgSuperManager_createCoupling(short fromLabel, short toLabel, float strength, float phaseDiff, CpgSuperManager_t* manager);

/**
 * Handles CPG messages
 * protected method do not use from controller
 */
void _CpgSuperManager_handleMessage(CpgSuperMsg_t* msg, char channel, CpgSuperManager_t* manager);

/**
 * Update all CPGs if it is time
 * returns the number of CPGs updated
 * protected method do not use from controller
 */
int _CpgSuperManager_update(CpgSuperManager_t* manager);


#endif /* CPGSUPERMANAGER_H_ */
