/**
\file Gradient.h

Gradient.h a simple hop-count gradient.
// \todo Removeal or forgetting of gradient if seed turns off gradient
\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef GRADIENT_H_
#define GRADIENT_H_
#include <ase/infrastructure/Entity/Manager/Manager.h>
#include <ase/communication/Message.h>
#include <ase/infrastructure/Entity/Strategy/PeriodicStrategy/PeriodicStrategy.h>
#include <stdlib.h>

#define GRADIENT_UPDATED_EVENT "GRADIENT_UPDATED_EVENT"
/**
 * Contains the data of a gradient
 * \extends PeriodicStrategy_t
 */
typedef struct {
	PeriodicStrategy_t super;
	short label;
	bool seed;
	short value;
} Gradient_t;

/**
 * Contains the message data of a gradient
 * \extends Message_t
 */
typedef struct {
	Message_t super;
	short value;
} GradientMsg_t;

void Gradient_initialize(Gradient_t *gradient, short label, float hz);
bool Gradient_sendIfTime(Gradient_t *gradient, GradientMsg_t* msg);
short Gradient_updateFromValue(Gradient_t *gradient, short value);
short Gradient_getValue(Gradient_t* gradient);
void Gradient_setSeed(Gradient_t *gradient, bool seed, short value);
bool Gradient_isSeed(Gradient_t *gradient);
short Gradient_getlabel(Gradient_t *gradient);


#endif /* GRADIENT_H_ */


