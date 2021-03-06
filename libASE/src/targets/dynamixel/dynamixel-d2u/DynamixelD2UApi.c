#include <ase/DynamixelApi.h>
#include <ase/Dynamixel.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/tools/Timer/TimerManager.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>
#include <Windows.h>
    #include <stdio.h>
//--- Control Table Address ---
//EEPROM AREA
#define P_MODEL_NUMBER_L 		0
#define P_MODOEL_NUMBER_H 		1
#define P_VERSION 				2
#define P_ID 					3
#define P_BAUD_RATE 			4
#define P_RETURN_DELAY_TIME 	5
#define P_CW_ANGLE_LIMIT_L 		6
#define P_CW_ANGLE_LIMIT_H 		7
#define P_CCW_ANGLE_LIMIT_L 	8
#define P_CCW_ANGLE_LIMIT_H 	9
#define P_SYSTEM_DATA2 			10
#define P_LIMIT_TEMPERATURE 	11
#define P_DOWN_LIMIT_VOLTAGE 	12
#define P_UP_LIMIT_VOLTAGE 		13
#define P_MAX_TORQUE_L 			14
#define P_MAX_TORQUE_H 			15
#define P_RETURN_LEVEL 			16
#define P_ALARM_LED 			17
#define P_ALARM_SHUTDOWN 		18
#define P_OPERATING_MODE 		19
#define P_DOWN_CALIBRATION_L 	20
#define P_DOWN_CALIBRATION_H 	21
#define P_UP_CALIBRATION_L 		22
#define P_UP_CALIBRATION_H 		23
#define P_TORQUE_ENABLE 		(24)
#define P_LED 					(25)
#define P_CW_COMPLIANCE_MARGIN 	(26)
#define P_CCW_COMPLIANCE_MARGIN (27)
#define P_CW_COMPLIANCE_SLOPE 	(28)
#define P_CCW_COMPLIANCE_SLOPE 	(29)
#define P_GOAL_POSITION_L 		(30)
#define P_GOAL_POSITION_H 		(31)
#define P_GOAL_SPEED_L 			(32)
#define P_GOAL_SPEED_H 			(33)
#define P_TORQUE_LIMIT_L 		(34)
#define P_TORQUE_LIMIT_H 		(35)
#define P_PRESENT_POSITION_L 	(36)
#define P_PRESENT_POSITION_H 	(37)
#define P_PRESENT_SPEED_L 		(38)
#define P_PRESENT_SPEED_H 		(39)
#define P_PRESENT_LOAD_L 		(40)
#define P_PRESENT_LOAD_H 		(41)
#define P_PRESENT_VOLTAGE 		(42)
#define P_PRESENT_TEMPERATURE 	(43)
#define P_REGISTERED_INSTRUCTION	(44)
#define P_PAUSE_TIME 			(45)
#define P_MOVING 				(46)
#define P_LOCK 					(47)
#define P_PUNCH_L 				(48)
#define P_PUNCH_H 				(49)

static Dynamixel_t dyna;

static void checkForError(int actuator,int callerID) {
	if( dxl_get_result() != COMM_RXSUCCESS ) {
		ase_printf( "ERROR: Dynamixel #%i, ID=%i: ", callerID, dyna.actuators[actuator]);
		char* errorType;
		switch( dxl_get_result() ) 	{
			case COMM_TXFAIL:
				errorType = "COMM_TXFAIL";
				break;

			case COMM_TXERROR:
				errorType = "COMM_TXERROR" ;
				break;
			case COMM_RXFAIL:
				errorType = "COMM_RXFAIL";
				break;

			case COMM_RXWAITING:
				errorType = "COMM_RXWAITING";
				break;

			case COMM_RXTIMEOUT:
				errorType = "COMM_RXTIMEOUT";
				break;

			case COMM_RXCORRUPT:
				errorType = "COMM_RXCORRUPT";
				break;
			default:
				printf("Unknown error!!!\n");
				break;
		}
		ase_printf(errorType);
		ase_printf("\n");
		Event_t event; event.val_prt = errorType;
		EventManager_publish(DYNAMIXEL_ERROR_EVENT, &event);
	}
}

static void updateStates(int id) {
	int i;
	for(i=0;i<dyna.nActuators;i++) {
		bool moving = dynamixelApi_isMoving(i);
		if(moving != dyna.states[i].isMoving) {
			if(!moving) {
				Event_t event; event.val_int = i;
				EventManager_publish(DYNAMIXEL_ROTATION_DONE_EVENT, &event);
			}
			dyna.states[i].isMoving = moving;
		}
	}
}

bool dynamixelApi_setup(int baud, void (*msgHandler)(char*, char, char)) {
	EventManager_registerTopic(DYNAMIXEL_ERROR_EVENT);
	int res = dxl_initialize();
	if (res == 1) {
		ase_printf("SUCCESS: USB2Dynamixel opened!\n");
		dynamixelApi_setBaud(baud);
		dyna.msgHandler = msgHandler;
		dyna.logPos = false;
		EventManager_registerTopic(DYNAMIXEL_ROTATION_DONE_EVENT);
		dyna.updateTimer = TimerManager_createPeriodicTimer(100, 0, updateStates); //10hz updates
		return true;
	}
	else {
		char* errorType = "Failed to open USB2Dynamixel!";
		ase_printf("ERROR: %s \n",errorType);
		Event_t event; event.val_prt = errorType;
		EventManager_publish(DYNAMIXEL_ERROR_EVENT, &event);
		return false;
	}
}

bool dynamixelApi_connect(int actId) {
	dxl_ping(actId);
	if (dxl_get_result() == COMM_RXSUCCESS) 	{
		ase_printf("SUCCESS: Found Dynamixel with id = %i renamed to %i \n", actId, dyna.nActuators);
		int wValue = dxl_read_word(actId, P_MODEL_NUMBER_L);
		if (dxl_get_result() == COMM_RXSUCCESS)
			ase_printf("Model number:%d, ", wValue);
		int bValue = dxl_read_byte(actId, P_VERSION);
		if (dxl_get_result() == COMM_RXSUCCESS)
			ase_printf("Version:%d\n", bValue);
		dyna.actuators[dyna.nActuators] = actId;
		dynamixelApi_setWheelMode(dyna.nActuators, false);
		dyna.nActuators++;
		return true;
	}
	else {
		ase_printf("ERROR: Unable to connect to Dynamixel with id = %i\n", actId);
		return false;
	}
}
void dynamixelApi_setIsLoggingPos(bool isLogging) {
	dyna.logPos = isLogging;
}

int dynamixelApi_countActuators() {
	return dyna.nActuators;
}


void dynamixelApi_setBaud(int baud) {
	dyna.baud = baud;
	dxl_set_baud(dyna.baud);
}

bool dynamixelApi_isWheelMode(int actuator) {
	return dyna.states[actuator].wheelMode;
}

void dynamixelApi_setWheelMode(int actuator, bool wheel) {
	if(wheel) {
		dynamixelApi_setAngleLimits(actuator, 0, 0);
		dyna.states[actuator].wheelMode = true;
	}
	else {
		dynamixelApi_setAngleLimits(actuator, 0, 1023);
		dyna.states[actuator].wheelMode = false;
	}
}
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setAngleLimits(int actuator, int minAngle, int maxAngle) {
	dxl_write_word(dyna.actuators[actuator], P_CW_ANGLE_LIMIT_L, minAngle);
	checkForError(actuator,1);
	dxl_write_word(dyna.actuators[actuator], P_CCW_ANGLE_LIMIT_L, maxAngle);
	checkForError(actuator,2);
}

/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setTemperatureLimit(int actuator, int maxTemperature) {
	dxl_write_word(dyna.actuators[actuator], P_LIMIT_TEMPERATURE, maxTemperature);
	checkForError(actuator,3);
}
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setVoltageLimits(int actuator, int minVoltage, int maxVoltage) {
	dxl_write_word(dyna.actuators[actuator], P_UP_LIMIT_VOLTAGE, minVoltage); //TODO byte or word?
	checkForError(actuator,4);
	dxl_write_word(dyna.actuators[actuator], P_DOWN_LIMIT_VOLTAGE, maxVoltage);
	checkForError(actuator,5);
}
/**
 * NOTE: Careful writes to EEPROM (limited erase/write cycles)
 */
void dynamixelApi_setMaxTorque(int actuator, int maxTorque) {
	dxl_write_word(dyna.actuators[actuator], P_MAX_TORQUE_L, maxTorque);
	checkForError(actuator,6);
}

//////////START OF RAM AREA//////////////

void dynamixelApi_setTorqueEnable(int actuator, bool enable) {
	dxl_write_byte(dyna.actuators[actuator], P_TORQUE_ENABLE, 0);
	checkForError(actuator,7);
}
void dynamixelApi_setLed(int actuator, bool led) {
	dxl_write_byte(dyna.actuators[actuator], P_LED, led);
	checkForError(actuator,8);
}

/**
 * Compliance Margin are the areas where output torque is 0.
 * Compliance Slope are the areas where output torque is reduced when they are getting close to Goal Position.
 * slope [1,254]
 * margin [0,254]
 */
void dynamixelApi_setCompliance(int actuator, char margin, char slope) {
	dxl_write_byte(dyna.actuators[actuator], P_CW_COMPLIANCE_MARGIN, margin);
	checkForError(actuator,9);
	dxl_write_byte(dyna.actuators[actuator], P_CCW_COMPLIANCE_MARGIN, margin);
	checkForError(actuator,10);
	dxl_write_byte(dyna.actuators[actuator], P_CW_COMPLIANCE_SLOPE, slope);
	checkForError(actuator,11);
	dxl_write_byte(dyna.actuators[actuator], P_CCW_COMPLIANCE_SLOPE, slope);
	checkForError(actuator,12);
}


void dynamixelApi_setGoalPos(int actuator, int pos) {
	if(dynamixelApi_isWheelMode(actuator)) printf("WARNING: Position control in Dynamixel only possible in non-wheel model\n");
	dxl_write_word(dyna.actuators[actuator], P_GOAL_POSITION_L, pos );
	if(dyna.logPos) {
		if(dyna.posLog==0) {
			char name[100];
			sprintf(name, "SetPos%li.log",time(NULL));
			dyna.posLog = fopen(name,"w");
		}
		fprintf(dyna.posLog,"%f \t %i \t %i \t %i\n",getLocalTime(), actuator, pos, dynamixelApi_getPosition(actuator));
		fflush(dyna.posLog);
	}
	checkForError(actuator,13);
}

void dynamixelApi_setMovingSpeed(int actuator, int speed) {
	if(!dynamixelApi_isWheelMode(actuator)) printf("WARNING: Torque control in Dynamixel only possible in wheel model\n");
	dxl_write_word(dyna.actuators[actuator], P_GOAL_SPEED_L, speed);
	checkForError(actuator,14);
}

int dynamixelApi_getSpeed(int actuator) {
	int temp = dxl_read_word( dyna.actuators[actuator], P_PRESENT_SPEED_L );
	checkForError(actuator,15);
	int sign = (temp&&1<<10)?-1:1;
	int ratio = 0x03FF&&temp;
	return sign * ratio;
}

void dynamixelApi_setTorqueLimit(int actuator, int maxTorque) {
	dxl_write_word(dyna.actuators[actuator], P_TORQUE_LIMIT_L, maxTorque);
	checkForError(actuator,16);
}

int dynamixelApi_getPosition(int actuator) {
	long start = getLocalMsTime();
	int presentPos = dxl_read_word( dyna.actuators[actuator], P_PRESENT_POSITION_L);
	long time = getLocalMsTime()-start;
	if(time>1) {
	//	printf("time taken to comm = %i ms\n ",getLocalMsTime()-start);
	}

	checkForError(actuator,17);
	return presentPos;
}

bool dynamixelApi_isAtValidAngle(int actuator, bool dir) {
	dynamixelApi_getPositionEstimate(actuator, dir);
	return dyna.estimates[actuator].estimatonState==0;
}

int dynamixelApi_getPositionEstimate(int actuator, bool dir) {
	int pos = dynamixelApi_getPosition(actuator);
	long currentTime = getLocalMsTime();
	int deltaT = currentTime-dyna.estimates[actuator].lastUpdate;
	if(dyna.estimates[actuator].estimatonState==0) { //normal mode
		dyna.estimates[actuator].posEstimate = pos;
		if((!dir && pos==1023) || (dir && pos==0)) dyna.estimates[actuator].estimatonState++;
	}
	else if(dyna.estimates[actuator].estimatonState==1) { //entering blind mode
		dyna.estimates[actuator].posEstimate += ((dir)?-1:1)*deltaT*1024*0.111f*dynamixelApi_getSpeed(actuator)/(60*1000);
		if((!dir && pos==0) || (dir && pos==1023)) dyna.estimates[actuator].estimatonState++;
	}
	else if(dyna.estimates[actuator].estimatonState==2) { //waiting to leave blind mode
		dyna.estimates[actuator].posEstimate += ((dir)?-1:1)*deltaT*1024*0.111f*dynamixelApi_getSpeed(actuator)/(60*1000);
		if((!dir && pos>0) || (dir && pos<1023)) {
			if(!(pos>100&&pos<(1023-100))) {
				dyna.estimates[actuator].estimatonState=0;
				dyna.estimates[actuator].posEstimate = pos;
			}
			else {
				//printf("spurious reading when crossing center\n");
				//spurious reading when crossing center
			}
		}
	}
	dyna.estimates[actuator].lastUpdate = currentTime;
	if(dyna.estimates[actuator].posEstimate>(1023+170)) {
		return 1023+170;
	}
	if(dyna.estimates[actuator].posEstimate<0) {
		if(dyna.estimates[actuator].posEstimate<-170) return 1023;
		else return 1023+170+dyna.estimates[actuator].posEstimate;
	}
	return dyna.estimates[actuator].posEstimate;
}

int dynamixelApi_getLoad(int actuator) {
	int load = dxl_read_word( dyna.actuators[actuator], P_PRESENT_LOAD_L);
	checkForError(actuator,18);
	int sign = (load&&1<<10)?-1:1;
	int ratio = 0x03FF&&load;
	return sign * ratio;
}

int dynamixelApi_getVoltage(int actuator) {
	int temp = dxl_read_byte( dyna.actuators[actuator], P_PRESENT_VOLTAGE );
	checkForError(actuator,19);
	return temp;
}

int dynamixelApi_getTemperature(int actuator) {
	int temp = dxl_read_byte( dyna.actuators[actuator], P_PRESENT_TEMPERATURE );
	checkForError(actuator,20);
	return temp;
}

bool dynamixelApi_isMoving(int actuator) {
	int moving = dxl_read_byte( dyna.actuators[actuator], P_MOVING);
	checkForError(actuator,21);
	return moving==1;
}

void dynamixelApi_setPunch(int actuator, int punch) {
	dxl_write_word(dyna.actuators[actuator], P_PUNCH_L, punch);
	checkForError(actuator,22);
}


void dynamixelApi_wheelMove(int actuator, int torque, bool dir) {
	int command = (dir<<10)|(0x03FF&torque);
	dxl_write_word(dyna.actuators[actuator], P_GOAL_SPEED_L, command);
	checkForError(actuator,23);
}
//10 9 8 7 6 5 4 3 2 1 0
//1  1 0 0 0 0 0 0 0 0 0

bool started;
float startTime;
float dynamixelApi_getTime() {
	SYSTEMTIME st;
	GetSystemTime(&st);
	float time =60.0f*st.wMinute+st.wSecond+st.wMilliseconds/1000.0f;
	if(!started) {
		started=true;
		startTime=time;
	}
	return time-startTime;
	//return 2.3f*((float)clock())/((float)CLOCKS_PER_SEC);
}
long dynamixelApi_getMsTime() {
	return (long)(1000*dynamixelApi_getTime());
}

int dynamixelApi_sendMessage(char* message, char messageSize, char connector) {
	ase_printf("Warning: dynamixelApi_sendMessage not implemented\n");
	return 0;
}

int dynamixelApi_sendMessageToAll(char* message, char messageSize){
//	ase_printf("Warning: dynamixelApi_sendMessageToAll not implemented\n");
	return 0;
}

int dynamixelApi_sendMessageToAllExcept(char* message, char messageSize, char except) {
	ase_printf("Warning: dynamixelApi_sendMessageToAllExcept not implemented\n");
	return 0;
}

int dynamixelApi_getHardwareID() {
	return 0;
}

long dynamixelApi_getRandomSeed() {
	return time(NULL);
}

void dynamixelApi_terminate() {
	dxl_terminate();
}

/*void atronApi_handleMessage(char* eventType, char* messageData) {
	char* message = strtok(messageData," "); //handleMessage string
	int messageSize = atoi(strtok(NULL, " "));
	int channel = atoi(strtok(NULL, " "));
	stringToCharArray(message, atronApi_msgReceiveBuffer);
	atronApi_msgHandler(atronApi_msgReceiveBuffer, messageSize, channel);
}*/
