/**
\file AtronStatus.h

AtronStatus.h monitors the status of an ATRON module and can print it for simplified debugging

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
\note This software is distributed under the BSD open-source license.
*/

#ifndef ATRONSTATUS_H_
#define ATRONSTATUS_H_

/**
 * Initialize the status (must be called before print)
 */
void AtronStatus_init();

/**
 * Print on serial interface or zig bee a status of the state of the module
 */
void AtronStatus_print();

int AtronStatus_getTotalIRMsgCount();
int AtronStatus_getTotalXbeeMsgCount();

#endif /*ATRONSTATUS_H_*/
