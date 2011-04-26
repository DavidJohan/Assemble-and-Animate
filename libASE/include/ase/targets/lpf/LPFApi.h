/**
\file LPFApi.h

 LPFApi.h provides an interface for an controller, controlling an LEGO Power Functions programmer

\author David Johan Christensen, Center for Playware, Technical University of Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef LPFAPI_H_
#include <stdbool.h>

#include  "stdconst.h"
#include  "m_sched.h"
#include  "c_cmd.h"
//#include  "c_timer.h"
#include  "c_io.h"
#include  "c_oi.h"
#include  "m_sched.h"
#include  "d_led.h"
#include  "d_mem.h"
#include  "lpf3.h"


/**
 * Initialize the API (important in the case of soft reset)
 */
void LPFApi_init();

/**
Get the current real time in seconds since the programmer was turned on
*/
float LPFApi_getTime();

/**
Get the current real time in milli seconds since the module was turned on
*/
long LPFApi_getMsTime();

/**
Send a message on a given connector
*/
int LPFApi_sendMessage(char* message, char messageSize, char connector); //byte as boolean

/**
Send a message to all others on the same bus?
*/
int LPFApi_sendMessageToAll(char* message, char messageSize);

/**
Send a message to all except one
*/
int LPFApi_sendMessageToAllExcept(char* message, char messageSize, char except);

/**
Get "fixed" hardware ID for this controller
 */
int LPFApi_getHardwareID();

/**
 * If anything is connected to the connector and if it is intelligent (can communicate)
 */
bool LPFApi_isConnectorIntelligent(int connector);

/**
Get a random seed
*/
long LPFApi_getRandomSeed();


/**
 * Counts and return the number of (non-intelligent) motors connected to the programmer
 */
int LPFApi_getNumberOfMotors();
int LPFApi_getNumberOfDetectSensors();
int LPFApi_getNumberOfLightSensors();
int LPFApi_getNumberOfTiltSensors();
int LPFApi_getNumberOfTiltMemsSensors();
int LPFApi_getNumberOfRGBLights();
int LPFApi_getNumberOfDisplays();

/**
 * Make the motor move with a given dir [FALSE=counterclockwise, TRUE=clockwise]
 * and power level [0-100]
 */
bool LPFApi_setMotorPower(int index, signed char power);
bool LPFApi_getDetectSensor(int index, signed char*  result);
bool LPFApi_getLightSensor(int index, signed char*  result);
bool LPFApi_getTiltSensor(int index, signed char*  result);
bool LPFApi_getTiltMemsSensor(int index, signed char*  result);
bool LPFApi_setRGBLight(int index, unsigned char r, unsigned char g, unsigned char b);
//bool LPFApi_setDisplay(int index, int x, int y, int color);
bool LPFApi_printOnDisplay(int index, int x, int y, char asciiVal);
/**
 * Turn on/off the red led on the programmer (is often overwritten in firmware)
 */
void LPFApi_setLedRed(bool on);

/**
 * Turn on/off the green led on the programmer (is often overwritten in firmware)
 */
void LPFApi_setLedGreen(bool on);
int ASE_LPF_getMessage(char* buffer, int maxSize);
void LPFApi_setMessage(char* data, int length);
void cASE1ms();
#define LPFAPI_H_
#endif
