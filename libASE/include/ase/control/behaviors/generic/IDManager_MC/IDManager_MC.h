/**
 * \file IDManager_MC.h
 * IDManager_MC.h enables a remote PC running ModularCommander to set a sofware ID on modules,
 * by making the mapping hardwareID->softwareID. This is done in a transparent way from the application.
 *
 * \see IDContainer.h
 * \see MCManager.h
 * \date June 11, 2009
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 */


#ifndef MCIDMANAGER_H_

/**
 * Installs an call-back function, which listens for messages from
 * ModularCommander (MC) which contains an sofware ID.
 * The ID received from MC is put in IDContainer.h
 */
void IDManager_MC_init();

#define MCIDMANAGER_H_

#endif /* MCIDMANAGER_H_ */
