/**
\file Entity.h
Entity.h is a top-level construct for ASE. Most strategies extend it. It contains a label.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maersk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef ENTITY_H_
#define ENTITY_H_
#include <stdbool.h>
#include <stdint.h>

#define NO_LABEL			1024

/**
 * Entity is ...
 *
 */
typedef struct {
	bool initialized:1;
	short label;
} Entity_t;

/**
 *Initialize this entity
 */
void Entity_init(Entity_t* entity, short label);

/**
 *Get the label of this entity
 */
short Entity_getLabel(Entity_t* entity);

/**
 *Set the label of this entity
 */
void Entity_setLabel(Entity_t* entity, short label);

/**
 * If the entity has been initialized
 */
bool Entity_isInitialized(Entity_t* entity);


#endif /* STRATEGY_H_*/
