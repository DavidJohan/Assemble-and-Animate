/**
 * \file IDContainer.h
 * IDContainer.h is a simple container of soft id (assigned at runtime), in
 * contrast to the ID of the hardware.
 *
 * \date March 8, 2009
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */


#ifndef IDCONTAINER_H_

typedef struct {
	int softID;
} IDContainer_t;

/**
 * Initialize the ID container
 */
void IDContainer_init();
/**
 * Set the software ID
 */
void IDContainer_setSoftID(int ID);
/**
 * Get the software ID
 */
int IDContainer_getSoftID();

#define IDCONTAINER_H_

#endif /* IDCONTAINER */
