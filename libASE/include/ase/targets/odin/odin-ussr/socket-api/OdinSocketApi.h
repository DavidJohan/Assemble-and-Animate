/**
\file OdinSocketApi.h

 OdinSocketApi.h provides an interface for an Odin module simulated in the Unified Simulator for Self-Reconfigurable Robots (USSR). API calls to the modules are communicated to USSR though a socket. Similar messages (asynchronous events) to the moduels are send from USSR to the module controller through a seperate socket. In total two sockets are used per module. Initially a call to odinApi_setup(...) must be made. This API builds on top of CommandSender.h and EventListner.h.

\author David Johan Christensen, Modular Robotics Lab, AdapTronics Group, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef ODINSOCKETAPI_H_


/*void setup();
boolean isActuating();
void actuate(float pos);
void disactivate();
float getTime();
String getName();
void setColor(float r, float g, float b);
float getActuatorPosition();
boolean isConnected(int i);
int getDebugID();
String getType();
void yield();
void delay(long ms);
char read(String name);
byte sendMessage(byte[] message, byte messageSize, byte connector);
void handleMessage(byte[] message, int messageSize, int channel);*/

// \todo revise and generalize this API, which function are needed for different types of odin modules and how can it easely be extended?
// \todo: Idea: One general odin API (much like this) and then specific APIs for each module type - should be reflected in USSR...
/**
Setup the API, two free socket ports portRC and portEvent must be provided for commands and events respectively.
Also a host (usually LOCALHOST) and a pointer to a function (e.g. handleMessage(char* message, char messageSize, char channel)) that will handle messages received from other modules.
*/
void odinApi_setup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char));

/**
If the last request to send a command was send correctly
*/
int odinApi_wasSend();

/**
Gets the name of the module is such a label has been set in USSR
*/
char* odinApi_getName(char* returnString);//String

/**
If the actuator is currently actuating or if it is disactivated
*/
int odinApi_isActuating();

/**
Actuate actuator towards a goal, from 0 to 1
*/
void odinApi_actuate(float pos);

/**
Turn off actuator (make it passive)
*/
void odinApi_disactivate();

/**
Get the current simulation (USSR) real time in seconds since the simulation started
*/
float odinApi_getTime();

/**
Get the position of the main actuator
*/
float odinApi_getActuatorPosition();

/**
Test if a specific connector is currently connected
*/
int odinApi_isConnected(int i);

/**
Yield the module, that is wait one physicstimestep in the USSR simulator
*/
void odinApi_yield();

/**
Wait ms milliseconds in the USSR simulator
*/
void odinApi_delay(long ms);

/**
Send a message on a given connector
*/
int odinApi_sendMessage(char* message, char messageSize, char connector); //byte as boolean



#define ODINSOCKETAPI_H_
#endif /* ODINSOCKETAPI_H_ */
