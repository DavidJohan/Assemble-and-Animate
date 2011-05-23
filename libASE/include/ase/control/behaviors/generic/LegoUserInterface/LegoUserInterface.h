/**
 * \file LegoUserInterface.h
 * This file provides ...
 *
 * \date May 2011
 * \author David Johan Christensen, Center for Playware, Technical University of Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef LEGOUSERINTERFACE_
#define LEGOUSERINTERFACE_

#define LUI_SET_RC_STATE			1
#define LUI_UPDATE_DEVICELIST		2
#define LUI_UPDATE_READ_DEVICELIST	3
#define LUI_SETUP					4
#define LUI_UPDATE_STATELIST		5
#define LUI_SELECTED_BEHAVIORS 		6

Subsumption_t* LUI_getSubsumptionProcess();
void LUI_init();

#endif
