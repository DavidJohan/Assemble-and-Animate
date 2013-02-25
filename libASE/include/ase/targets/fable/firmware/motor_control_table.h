/*! \file motor_control_table.h
    \author Walter Gambelunghe
    \copyright GNU Public License V3
    \brief Symbolic definitions for the dynamixel internal RAM memory addresses.
    See <a href="http://support.robotis.com/en/product/dynamixel/ax_series/dxl_ax_actuator.htm"> Dynamixel documentation </a> for detailed description
 */

#ifndef __MOTOR_CONTROL_TABLE
#define __MOTOR_CONTROL_TABLE

//DEFINE	NAME				ADDRESS	//HEX	 COMMENT 				RW 	INITIAL VALUE
#define TORQUE_ENABLE 		24 		//(0X18) 	Torque On/Off 			RW 	0 (0X00)

#define LED 				25		// (0X19)	LED On/Off				RW	0 (0X00)

#define CW_COMPLIANCE_MARGIN 	26 		//(0X1A)	CW Compliance margin		RW	1 (0X01)

#define CCW_COMPLIANCE_MARGIN 27 		//(0X1B)	CCW Compliance margin		RW	1 (0X01)

#define CW_COMPLIANCE_SLOPE	28		//(0X1C)	CW Compliance slope			RW	32 (0X20)

#define CCW_COMPLIANCE_SLOPE 29 		//(0X1D)	CCW Compliance slope		RW	32 (0X20)

#define GOAL_POSITION_L		30 		//(0X1E)	Lowest byte of Goal Position	RW 	NONE

#define GOAL_POSITION_H 	31		//(0X1F)	Highest byte of Goal Position	RW 	NONE

#define MOVING_SPEED_L		32 		//(0X20)	Lowest byte of Moving Speed	RW	NONE

#define MOVING_SPEED_H		33 		//(0X21)	Highest byte of Moving Speed	RW	NONE

#define TORQUE_LIMIT_L		34 		//(0X22)	Lowest byte of Torque Limit	RW	ADD14(MAXTORQUEL IN EE)

#define TORQUE_LIMIT_H		35 		//(0X23)	Highest byte of Torque Limit	RW	ADD15(MAXTORQUEH IN EE)

#define PRESENT_POSITION_L	36 		//(0X24)	Lowest byte of Current Position	R	NONE

#define PRESENT_POSITION_H	37		//(0X25)	Highest byte of Current Position	R	NONE

#define PRESENT_SPEED_L		38 		//(0X26)	Lowest byte of Current Speed	R	NONE

#define PRESENT_SPEED_H		39 		//(0X27)	Highest byte of Current Speed	R	NONE

#define PRESENT_LOAD_L		40 		//(0X28)	Lowest byte of Current Load	R	NONE

#define PRESENT_LOAD_H		41		//(0X29)	Highest byte of Current Load	R	NONE

#define PRESENT_VOLTAGE		42		//(0X2A)	Current Voltage			R	NONE

#define PRESENT_TEMPERATURE	43 		//(0X2B)	Current Temperature			R	NONE

#define REGISTERED			44		//(0X2C)	Means if Instruction is registered	R	0 (0X00)

#define MOVING				46 		//(0X2E)	Means if there is any movement	R	0 (0X00)

#define LOCK				47		//(0X2F)	Locking EEPROM			RW	0 (0X00)

#define PUNCH_L				48 		//(0X30)	Lowest byte of Punch			RW	32 (0X20)

#define PUNCH_H				49 		//(0X31)	Highest byte of Punch		RW	0 (0X00)

#endif	// __MOTOR_CONTROL_TABLE
