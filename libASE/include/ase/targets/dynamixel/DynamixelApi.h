/**
\file DynamixelApi.h

 DynamixelApi.h provides an interface for an controller, controlling an number of Dynamixel Actuators

\author David Johan Christensen, Modular Robotics Lab, Maresk Institute, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef DYNAMIXELAPI_H_
#include <stdbool.h>
#include <stdio.h>
#include <ase/config/ConfigASE.h>
#include <ase/tools/Timer/TimerManager.h>

#define DYNAMIXEL_ROTATION_DONE_EVENT	"DYNAMIXEL_ROTATION_DONE_EVENT"
#define DYNAMIXEL_ERROR_EVENT	"DYNAMIXEL_ERROR_EVENT"

typedef struct {
	int posEstimate;
	long lastUpdate;
	int estimatonState;
} DynamixelPosEstimate_t;

typedef struct {
	bool wheelMode;
	bool isMoving;
} DynamixelState_t;

typedef struct {
	int baud;
	int nActuators;
	int actuators[MAX_ACTUATORS];
	DynamixelState_t states[MAX_ACTUATORS];
	DynamixelPosEstimate_t estimates[MAX_ACTUATORS];
	Timer_t* updateTimer;
	FILE* posLog;
	bool logPos;
	void (*msgHandler)(char*, char, char);
} Dynamixel_t;


/**
Setup actuators
*/
bool dynamixelApi_setup(int baud, void (*msgHandler)(char*, char, char));

/**
 * Set baud rate of the Dynamixels (note, not in bps - see dynamixel manual)
 */
void dynamixelApi_setBaud(int baud);

/**
 * Set if actuator is in wheel mode (unlimited rotation) or in
 * Joint mode (0 to 300 degrees controlled rotation)
 */
void dynamixelApi_setWheelMode(int actuator, bool wheel);

/**
 * Is actuator set to be a wheel or not?
 */
bool dynamixelApi_isWheelMode(int actuator);

/**
 * Log set position to posLog*.log
 *
 */
void dynamixelApi_setIsLoggingPos(bool isLogging);

/**
 * Connect to an dyanmixel actuator with a specific ID
 */
bool dynamixelApi_connect(int actId);

/**
 * Count the number of dynamixel actuators connected to this controller (index 0...n)
 */
int dynamixelApi_countActuators();


/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setAngleLimits(int actuator, int minAngle, int maxAngle);
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setTemperatureLimit(int actuator, int maxTemperature);
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setVoltageLimits(int actuator, int minVoltage, int maxVoltage);
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setMaxTorque(int actuator, int maxTorque);
//////////START OF RAM AREA//////////////
void dynamixelApi_setTorqueEnable(int actuator, bool enable);
void dynamixelApi_setLed(int actuator, bool led);
/**
 * Compliance Margin are the areas where output torque is 0.
 * Compliance Slope are the areas where output torque is reduced when they are getting close to Goal Position.
 * slope [1,254]
 * margin [0,254]
 */
void dynamixelApi_setCompliance(int actuator, char margin, char slope);
/**
Rotate a joint to a specific position [0,1024]
*/
void dynamixelApi_setGoalPos(int actuator, int pos);
void dynamixelApi_setMovingSpeed(int actuator, int speed);
int dynamixelApi_getSpeed(int actuator);
void dynamixelApi_setTorqueLimit(int actuator, int maxTorque);
/**
 * Get the position of the actuator in the interval [0;1024]
 */
int dynamixelApi_getPosition(int actuator);
/**
* Get the full 360 degree position estimate in the interval [0;1193]
* Note: For wheel mode only
*/
int dynamixelApi_getPositionEstimate(int actuator, bool dir);
bool dynamixelApi_isAtValidAngle(int actuator, bool dir);
int dynamixelApi_getLoad(int actuator);
int dynamixelApi_getVoltage(int actuator);
int dynamixelApi_getTemperature(int actuator);
/**
If the specific actuator is currently moving
*/
bool dynamixelApi_isMoving(int actuator);
void dynamixelApi_setPunch(int actuator, int punch);
/**
Rotate the main joint continuously in the given direction.
*/
void dynamixelApi_wheelMove(int actuator, int torque, bool dir);

/**
Get the current real time in seconds since the module was turned on
*/
float dynamixelApi_getTime();

/**
Get the current real time in milli seconds since the module was turned on
*/
long dynamixelApi_getMsTime();

/**
Send a message on a given connector
*/
int dynamixelApi_sendMessage(char* message, char messageSize, char connector); //byte as boolean

/**
Send a message to all others on the same bus?
*/
int dynamixelApi_sendMessageToAll(char* message, char messageSize);

/**
Send a message to all except one
*/
int dynamixelApi_sendMessageToAllExcept(char* message, char messageSize, char except);

/**
Get "fixed" hardware ID for this controller
 */
int dynamixelApi_getHardwareID();

/**
Get a random seed
*/
long dynamixelApi_getRandomSeed();

/**
 * Terminate connection to dynamixel actuators
 */
void dynamixelApi_terminate();

#define DYNAMIXELAPI_H_
#endif /* DYNAMIXELAPI_H_ */
