/**
\file GradientManager.h

GradientManager.h manages simple hop-count gradients.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef GRADIENTMANAGER_H_
#include <ase/control/strategies/Gradient/Gradient.h>
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/tools/LinkedList/LinkedList.h>

typedef struct {
	Manager_t super;
	GradientMsg_t msg;
	listnode_t* gradients;
} GradientManager_t;

Gradient_t* GradientManager_createGradient(short label, float hz);
void GradientManager_startGradient(Gradient_t* gradient);
void GradientManager_stopGradient(Gradient_t* gradient);
void GradientManager_pauseGradient(Gradient_t* gradient);
#define GRADIENTMANAGER_H_

#endif /* GRADIENTMANAGER_H_ */
