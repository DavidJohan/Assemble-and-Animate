/**
\file MtranTosApi.h

 MTRANApi.h provides an interface for an MTRAN module.

\author Ulrik Pagh Schultz, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef MTRANTOSAPI_H_

#define MTRAN_DIR_POS TRUE
#define MTRAN_DIR_NEG FALSE

/**
Setup used for physical modules running tinyos
*/
void mtranApi_setup();

/**
Setup the API for USSR, two free socket ports portRC and portEvent must be provided for commands and events respectively.
Also a host (usually LOCALHOST) and a pointer to a function (e.g. handleMessage(char* message, char messageSize, char channel)) that will handle messages received from other modules.
*/
void mtranApi_ussrSetup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char));

/**
Get a random seed
*/
long mtranApi_getRandomSeed();

/**
Gets the name of the module (if set at compile time)
*/
char* mtranApi_getName(char* returnString);

/**
If the module is currently rotating
*/
int mtranApi_isRotating(int actuator);//boolean

/**
Rotate a joint to a specific radian position
*/
void mtranApi_rotateToDegree(int actuator,float rad,int dir);

/**
Rotate a joint to a specific position from 0 to 180
*/
void mtranApi_rotateDirToInDegrees(int actuator, int degrees,int dir);

/**
Get the current real time in seconds since the module was turned on
*/
float mtranApi_getTime();

/**
Get the current real time in milli seconds since the module was turned on
*/
long mtranApi_getMsTime();

/**
Get the position of the main joint in radians
*/
float mtranApi_getAngularPosition(int actuator);

/**
Get the position of the main joint in degrees
*/
int mtranApi_getAngularPositionDegrees(int actuator);

/**
Disconnect all connectors
*/
void mtranApi_disconnectAll();

/**
Connect all connectors
*/
void mtranApi_connectAll();

/**
Test whether a specific connector is able to connect, best effort based on avaliable information (no garantiees on physical modules)
*/
int mtranApi_canConnect(int i); //boolean

/**
Test whether a specific connector can disconnect, meaning that it is a male connector currently connected to another module.
*/
int mtranApi_canDisconnect(int i); //boolean

/**
Test if a specific connector is a male connector
*/
int mtranApi_isMale(int i); //boolean

/**
Attempt to connect this connector.
*/
void mtranApi_connect(int i);

/**
Attempt to disconnect this connector, will work if the connector is male and connected
*/
void mtranApi_disconnect(int i);

/**
Test if a specific connector is currently connected
*/
int mtranApi_isConnected(int i); //boolean

/**
Test is a specific connector is currently disconnected
*/
int mtranApi_isDisconnected(int i); //boolean

/**
Rotate the main joint continuously in the given direction.
*/
void mtranApi_rotateContinuous(int actuator,int vel);

/**
Cause the main joint to stop rotating and presumably break actively (the latter is not implemented yet)
*/
void mtranApi_centerBrake(int actuator);

/**
Causes the main joint to stop rotating
*/
void mtranApi_centerStop(int actuator);

/**
Test is another connector is within communication range
*/
int mtranApi_isOtherConnectorNearby(int connector); //boolean

/**
Test if there is an object in the proximity of a specific connector.
*/
int mtranApi_isObjectNearby(int connector); //boolean

/**
Read the value of the X tilt sensor.
*/
short mtranApi_getTiltX(); //byte

/**
Read the value of the Y tilt sensor.
*/
short mtranApi_getTiltY(); //byte

/**
Read the value of the Z tilt sensor.
*/
short mtranApi_getTiltZ(); //byte

/**
Yield the module, that is wait one physicstimestep in the USSR simulator (dummy on physical modules)
*/
void mtranApi_yield();

/**
Wait ms milliseconds
*/
void mtranApi_delay(long ms);

/**
Send a message on a given connector
*/
int mtranApi_sendMessage(char* message, char messageSize, char connector); //byte as boolean

/**
Send a message to all IR connectors
*/
int mtranApi_sendMessageToAll(char* message, char messageSize);

/**
Send a message to all IR connectors except one
*/
int mtranApi_sendMessageToAllExcept(char* message, char messageSize, char except);


/**
Get "fixed" hardware ID written to flash or set in USSR
 */
int mtranApi_getHardwareID();

/**
   Temporarily lock actuators in current position
*/
void mtranApi_lockActuators();

#define MTRANTOSAPI_H_
#endif /* MTRANTOSAPIR_H_ */
