/**
 * \file AbstractModuleApi.h
 * AbstractModuleApi.h provides a generic interface or hardware abstraction layer to control
 * many aspects of most modules.The goal is provide a method to write generic control
 * code which can be run on different platforms. All control strategies included in the
 * ASE library uses only this interface to access the modules.
 *
 * \author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
 * \note This software is distributed under the BSD open-source license.
 */

#ifndef ABSTRACTMODULEAPI_H_

#ifdef DUMMY
#define ase_printf(format, ...) printf(format, ##__VA_ARGS__);
#endif

#ifdef USSR
#define ase_printf(format, ...) printf(format, ##__VA_ARGS__);
#endif

#ifdef WEBOTS
#define ase_printf(format, ...) printf(format, ##__VA_ARGS__);
#endif

#ifdef D2U //Dynamixes2USB
#define ase_printf(format, ...) printf(format, ##__VA_ARGS__);
#endif

#ifdef TINYOS
#include <avr/pgmspace.h>
#define ase_printf(format, ...) printf_P(PSTR(format), ##__VA_ARGS__);
#endif

#ifdef FPGA
#define ase_printf(format, ...)  printf(format, ##__VA_ARGS__);
#endif

#ifdef LPF
#include <ase/targets/non-public/lpf/LPFPrintf.h>
#define ase_printf(...) lpf_printf(__VA_ARGS__);
#endif

#ifdef LMF
#define ase_printf(...) //lpf_printf(__VA_ARGS__);
#endif

#ifdef CM510 //Dynamixel controller
#define ase_printf(format, ...) printf(format, ##__VA_ARGS__);
#endif

/**
Get the time in seconds since the module was started
*/
float getLocalTime();

/**
Get the time in milliseconds since the module was started
*/
long getLocalMsTime();

/**
Get a random number seed, which is not the same on every module
*/
long getRandomSeed();


/**
Send a message to all neighbor modules.
Returns the number of messages send.
*/
int sendMessageToAllNeighbors(char* msg, int length);

/**
Send a message to neighbor module(s) on a given channel.
Returns the number of messages send.
*/
int sendMessageToNeighbor(char* msg, int length, int channel);

/**
Send a message to all neighbor module(s) except one on a given channel.
Returns the number of messages send.
*/
int sendMessageToAllExcept(char* msg, char length, char except);

/**
Returns the number of "local" channels. Neighbor channels can be assumed
to run from 0 to getNumberOfNeighborChannels()-1.
*/
int getNumberOfNeighborChannels();

/**
Returns if a modules can communication with another module using a given channel.
The type of returns can be: 0-no other, 1-yes (one or several other modules), 2-undefined (e.g.
channel does not exist or it is impossible for the module to know).
*/
int hasNeighbor(int channel);

/**
Returns the modules hardware ID. Note that for some system it might not always be present or
it might not be globally unique.
*/
int getHardwareID();


/**
Set the target position (scaled 0.0-1.0) of the actuator with an index (0, 1, 2,...)
returns 1 if successful
*/
int setActuatorPosition(float position, int actuator);

/**
Get the current position (scaled 0.0-1.0) of the actuator with an index (0, 1, 2,...)
*/
float getActuatorPosition(int actuator);

/**
Returns the number of actuators on the module.
*/
int getNumberOfActuators();

/**
Returns the number of sensors on the module.
*/
int getNumberOfSensors();

/**
Get current/lates sensor (scaled 0.0-1.0) of the sensor with an index (0, 1, 2,...)
*/
float getSensorValue(int sensor);


#define ABSTRACTMODULEAPI_H_

#endif /* ABSTRACTMODULEAPI_H_ */

/** \mainpage ASsemble and AnimatE Control Library (ASE)

\section intro_sec Introduction
The overall objective of the ASE project is to develop a control framework for modular robots that
enables rapid development and deployment of robots. To fulfill this goal, the ASE control
library will be:
- Extendible with new algorithms and support of new robot systems.
- Flexible to enable a user to manipulate the control system of a modular robot as easy as the user can manipulate and reconfigure its modules.
- Adaptive to automatically adapt the robot behavior to match the robot's configuration, environment, and task.

More details about the ASE project including download of library can be found here:

http://modular.mmmi.sdu.dk/wiki/ASE

\section install_sec Installation
A tutorial on how to install, copile and extend the library is given here:
http://modular.mmmi.sdu.dk/wiki/ASE_Tutorial
*/

