/**
\file AtronApi.h

 AtronApi.h provides an interface for an ATRON module.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef ATRONAPI_H_

/**
Setup used for physical modules running tinyos
*/
void atronApi_setup();

/**
Setup the API for USSR, two free socket ports portRC and portEvent must be provided for commands and events respectively.
Also a host (usually LOCALHOST) and a pointer to a function (e.g. handleMessage(char* message, char messageSize, char channel)) that will handle messages received from other modules.
*/
void atronApi_ussrSetup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char));


/**
Rotate the module to a home position
*/
void atronApi_home();

/**
Set the Leds on the north hemisphere (8 bits)
*/
void atronApi_setLeds(int i);

/**
Get the Leds on the north hemisphere (8 bits)
*/
int atronApi_getLeds();


/**
Get a random seed
*/
long atronApi_getRandomSeed();


/**
Gets the name of the module (if set at compile time)
*/
char* atronApi_getName(char* returnString);

/**
If the module is currently rotating
*/
int atronApi_isRotating();//boolean

/**
Get the main joint position in terms of the current hemisphere (0-3 both inclusive)
*/
int atronApi_getJointPosition();

/**
Rotate the main joint to a specific joint position in terms of the hemisphere (0-3 both inclusive)
*/
void atronApi_rotate(int dir);

/**
Rotate the main joint a number of degrees
*/
void atronApi_rotateDegrees(int degrees);

/**
Rotate the main joint to a specific degree position
*/
void atronApi_rotateToDegreeInDegrees(int degrees);

/**
Rotate the main jount to a specific radian position
*/
void atronApi_rotateToDegree(float rad);

/**
Rotate the main joint to a specific degree position passing by a specic position
*/
void atronApi_rotateToViaInDegrees(int goalDegree, int viaDegree);

/**
Rotate the main joint to a specific degree position in a given direction (clockwise or counterclockwise)
*/
void atronApi_rotateDirToInDegrees(int to, int dir);


/**
Get the current real time in seconds since the module was turned on
*/
float atronApi_getTime();

/**
Get the current real time in milli seconds since the module was turned on
*/
long atronApi_getMsTime();

/**
Get the position of the main joint in radians
*/
float atronApi_getAngularPosition();

/**
Get the position of the main joint in degrees
*/
int atronApi_getAngularPositionDegrees();

/**
Disconnect all connectors
*/
void atronApi_disconnectAll();

/**
Connect all connectors
*/
void atronApi_connectAll();

/**
Test whether a specific connector is able to connect, best effort based on avaliable information (no garantiees on physical modules)
*/
int atronApi_canConnect(int i); //boolean

/**
Test whether a specific connector can disconnect, meaning that it is a male connector currently connected to another module.
*/
int atronApi_canDisconnect(int i); //boolean

/**
Test if a specific connector is a male connector
*/
int atronApi_isMale(int i); //boolean

/**
Attempt to connect this connector.
*/
void atronApi_connect(int i);

/**
Attempt to disconnect this connector, will work if the connector is male and connected
*/
void atronApi_disconnect(int i);

/**
Test if a specific connector is currently connected
*/
int atronApi_isConnected(int i); //boolean

/**
Test is a specific connector is currently disconnected
*/
int atronApi_isDisconnected(int i); //boolean

/**
Rotate the main joint continuously in the given direction.
*/
void atronApi_rotateContinuous(float dir);

/**
Cause the main joint to stop rotating and presumably break actively (the latter is not implemented yet)
*/
void atronApi_centerBrake();

/**
Causes the main joint to stop rotating
*/
void atronApi_centerStop();

/**
Test is another connector is within communication range
*/
int atronApi_isOtherConnectorNearby(int connector); //boolean

/**
Test if there is an object in the proximity of a specific connector.
*/
int atronApi_isObjectNearby(int connector); //boolean

/**
Read the value of the X tilt sensor.
*/
short atronApi_getTiltX(); //byte

/**
Read the value of the Y tilt sensor.
*/
short atronApi_getTiltY(); //byte

/**
Read the value of the Z tilt sensor.
*/
short atronApi_getTiltZ(); //byte

/**
Yield the module, that is wait one physicstimestep in the USSR simulator (dummy on physical modules)
*/
void atronApi_yield();

/**
Wait ms milliseconds
*/
void atronApi_delay(long ms);

/**
Send a message on a given connector
*/
int atronApi_sendMessage(char* message, char messageSize, char connector); //byte as boolean

/**
Send a message to all IR connectors
*/
int atronApi_sendMessageToAll(char* message, char messageSize);

/**
Send a message to all IR connectors except one
*/
int atronApi_sendMessageToAllExcept(char* message, char messageSize, char except);


/**
Get "fixed" hardware ID written to flash or set in USSR
 */
int atronApi_getHardwareID();

/**
Get battery level of module in mV [0, 4200mV], below 3500mV is critically low
 */
int atronApi_getBatteryLevel();


#define ATRONAPI_H_
#endif /* ATRONAPIR_H_ */
