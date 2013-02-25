/*!
\file motor.h
\brief Interface to control the Dynamixel motors
\author Walter Gambelunghe
	\copyright GNU Public License V3
	\date 2012

	\file motor.h
	\details This is a wrapper library encapsulating the provided dynamixel library. It provides functions to easily control the motors.
		See the <a href="http://support.robotis.com/en/software/dynamixel_sdk/api_reference.htm"> Dynamixel SDK documentation </a> for further details.
*/
#ifndef __MOTOR_H
#define __MOTOR_H
///Symbolic definition for Wheel mode, to be used with #motor_set_mode and #motor_get_mode functions 
#define MOTOR_WHEEL_MODE 0
///Symbolic definition for Joint mode, to be used with #motor_set_mode and #motor_get_mode functions 
#define MOTOR_JOINT_MODE 1023

///Broadcast id, used to address all the motors
#define MOTOR_BROADCAST_ID 254

///Symbol for defining Clockwise spinning direction when in Wheel mode. Should be used with #motor_set_speed_dir function
#define MOTOR_CW 1
///Symbol for defining Counterclockwise spinning direction when in Wheel mode. Should be used with #motor_set_speed_dir function
#define MOTOR_CCW 0

///Symbol to define a blocking function request. Should be used with #motor_move, #motor_sync_move and #motor_set_position functions
#define MOTOR_MOVE_BLOCKING			1
///Symbol to define a non-blocking function request. Should be used with #motor_move, #motor_sync_move and #motor_set_position functions
#define MOTOR_MOVE_NON_BLOCKING		0

#include <avr/io.h>
#include "dynamixel.h"
#include "motor_control_table.h"

// *********************************************************************************
/** Function move a single motor to a desired position.
* This function can be used to move a motor to a certain position.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID
* \param[in]	motor_position The desired position.
* Valid arguments are 10 bits unsigned integers i.e. in the range [0;1023]
* \param[in]	blocking	Whether the function shuold wait for the position to be reached before returning
* Valid values are #MOTOR_MOVE_BLOCKING or #MOTOR_MOVE_NON_BLOCKING
* \note This function only works if the specified motor is in Joint mode. See #motor_set_mode and #motor_get_mode.
*/
void motor_move(char id, uint16_t motor_position, char blocking);

// *********************************************************************************
/** Function to set motor operating mode.
* This function can be used to set motor to either Wheel mode or Joint mode.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID
* \param[in] mode The desired mode to be set. Valid valueas are #MOTOR_WHEEL_MODE or #MOTOR_JOINT_MODE
*/
void motor_set_mode(char id ,int mode);

// *********************************************************************************
/** Function to get current motor operating mode.
* This function can be used to check the actual motor operating mode, either #MOTOR_WHEEL_MODE or #MOTOR_JOINT_MODE.
\par Example:
\code
if(motor_get_mode(1)==MOTOR_WHEEL_MODE)
	printf("Motor 1 is in wheel mode\n");
else
	printf("Motor 1 is in joint mode\n");

\endcode
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID
*\returns Returns the current operating mode, which should be checked against either #MOTOR_WHEEL_MODE or #MOTOR_JOINT_MODE
*/
int motor_get_mode(char id);

/** Function to set the desired speed. 
* \note Notice that the speed value also defines the spin direction. See #motor_set_speed_dir for a more intuitive interface.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
* \param [in] speed The desired speed.
* Speed value should be in the [0:1023] range for Clockwise direction or in the [1024:2047] range for Counterclocwise direction.
* Higher values correspond to higher speeds.
*\note This function only works if the specified motor is in Wheel mode. See #motor_set_mode and #motor_get_mode.
*/
void motor_set_speed(char id, int speed);

/** Function to set the desired speed and direction. 
*	This function can be used to set a motor spinning direction and speed in percentage.
\par Example: Set motor 1 to move at 100 percent speed clockwise
 \code
 	motor_set_speed_dir(1, 100, MOTOR_CW);
 \endcode 
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
* \param [in] percentage The desired speed in percentage.
* Valid values are in the range [0:100]
* \param [in] wise The desired direction. Should be either #MOTOR_CW or #MOTOR_CCW.
* \note This function only works if the specified motor is in Wheel mode. See #motor_set_mode and #motor_get_mode.
*/
void motor_set_speed_dir(char id, uint8_t percentage, char wise);

/** Function to get current speed. 
* This function can be used to read the current motor spinning speed.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
*\returns The current speed. The direction can be desumed by this value. 
* Speed values in the [0:1023] range correspond to Clockwise direction while values in the [1024:2047] range correspond to Counterclocwise direction.
* Higher values correspond to higher speeds.
* \note This function only works if the specified motor is in Wheel mode. See #motor_set_mode and #motor_get_mode.
*/
int motor_get_speed(char id);

/** Function to wait until the motor reaches a position
* It is used internally for the blocking #motor_move , #motor_set_position and #motor_sync_move functions.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
* \param [in] goal_position The position to be reached befor returning
*/
void motor_wait_finish(char id, uint16_t goal_position);

/** Function to set the desired position.
*This function can be used to set a motor to a desired position.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
* \param [in] motor_position The desired position in the [0:1023] range.
* \param[in]	blocking Whether the function shuold wait for the position to be reached before returning
* Valid values are #MOTOR_MOVE_BLOCKING or #MOTOR_MOVE_NON_BLOCKING
*\note This function only works if the specified motor is in Joint mode. See #motor_set_mode and #motor_get_mode.
*/
void motor_set_position(char id, uint16_t motor_position, char blocking);

/** Function to get current position.
* This function can be used to read the current motor position.
* \param [in] id The id of the motor to move. 
* Valid arguments are unsigned integer numbers or #MOTOR_BROADCAST_ID 
*\returns The current position, in the range [0:1023].
* \note This function only works if the specified motor is in Joint mode. See #motor_set_mode and #motor_get_mode.
*/
uint16_t motor_get_position(char id);

/** Function move several motors to the desired position at the same time.
* This function can be used to read the current motor position.
\par Example: move motor 1 to position 0 and motor 2 to position 1023 at the same time
\code
#define NUMBER_OF_MOTORS 2

uint8_t ids[2];
ids[0]=1;
ids[1]=2;

uint16_t positions[2];
positions[0]=0;
positions[1]=1023;

motor_sync_move( NUMBER_OF_MOTORS , ids, positions, MOTOR_MOVE_BLOCKING);
printf("both motors arrived to their goal position");

\endcode
* \param [in] size The number of motors to move.The following array parameters MUST be of this size.
* \param [in] id An array of unsigned integers specifying the ids. It must be of the size specified in the first argument.
* Valid values are unsigned integer numbers. 
*\param [in] position An array of unsigned integers specifying the target positions for the motors specified in the previous parameter.
It must be of the size specified in the first argument.
* \param[in]	blocking Whether the function shuold wait for the position to be reached before returning
* Valid values are #MOTOR_MOVE_BLOCKING or #MOTOR_MOVE_NON_BLOCKING
* \note This function only works if the specified motor is in Joint mode. See #motor_set_mode and #motor_get_mode.
*/
void motor_sync_move(const uint8_t size, const uint8_t * id, const uint16_t * position, const char blocking);

/** Function to print communication error status.
* This function can be used to output 
\par Example:
 \code
int result = dxl_get_result();       //check communication
	if (result != COMM_RXSUCCESS) //if communication failed
		PrintCommStatus(result);  //print error
 \endcode
* \param [in] CommStatus Should be the return value of dxl_get_result() function defined in dynamixel.h  
* \note This function was in the example code that came with the Dynamixel SDK
*/

void PrintCommStatus(int CommStatus);

/** Function to print motor error status (overheat,  input voltage error, etc...).
* \note This function was in the example code that came with the Dynamixel SDK
*/

void PrintErrorCode();

#endif //__MOTOR_H


