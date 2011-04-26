/**
\file Manager.h
Component.h is a top-level construct for ASE. Most managers extend it. It contains methods and variables
to manage components?.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maersk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef MANAGER_H_
#define MANAGER_H_
#include <stdbool.h>
#include <ase/infrastructure/Entity/Entity.h>

/**
 * This struct contains the variables and methods to manage components
 * \extends Entity_t
 */
typedef struct {
	Entity_t super;
} Manager_t;

void Manager_init(Manager_t* manager, short label);
bool Manager_isInitialized(Manager_t* manager);


#endif /* MANAGER_H_*/
