#include <ase/targets/roombot-webots.h>
#include <webots/robot.h>
#include <webots/servo.h>
#include <webots/emitter.h>
#include <webots/receiver.h>
#include <webots/connector.h>
#include <webots/accelerometer.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>


#define TIME_STEP 			32
#define TIME_MEDIUM_STEP 	32
#define TIME_SMALL_STEP 	16

static WbDeviceTag s1;
static WbDeviceTag m1;
static WbDeviceTag s2;
static WbDeviceTag connectors[10];
static WbDeviceTag radioEmitter;
static WbDeviceTag radioReceiver;
static WbDeviceTag accelerometer;
static WbDeviceTag irEmitters[10];
static WbDeviceTag irReceivers[10];
static void (*messageHandler)(char*, char, char);
static char* connectorNames[10] = {"C0X","C0Y","C0Z","C1Y","C1Z","C2Y","C2Z","C3X","C3Y","C3Z"};
static char* irEmitterNames[10] = {"E0X","E0Y","E0Z","E1Y","E1Z","E2Y","E2Z","E3X","E3Y","E3Z"};
static char* irReceiversNames[10] = {"R0X","R0Y","R0Z","R1Y","R1Z","R2Y","R2Z","R3X","R3Y","R3Z"};
static int myID;
static float energyConsumed=0;
static float oldAngle[3];


static WbDeviceTag getServoFromID(int servoID) {
	if(servoID ==0) return s1;
	else if(servoID ==1) return m1;
	else if(servoID ==2) return s2;
	else printf("Warning: no servo with ID =%i \n",servoID);
	return -1;
}

static void initServo(WbDeviceTag tag, double kp, double maxVelo) {
    wb_servo_set_control_p(tag, kp);
	wb_servo_set_velocity(tag, maxVelo);
	wb_servo_enable_position(tag, 1);
	wb_servo_enable_motor_force_feedback(tag, 1);
}

static void initServos() {
	s1=wb_robot_get_device("s1"); initServo(s1, 1000000.0f, 2.61f); //10
	s2=wb_robot_get_device("s2"); initServo(s2, 1000000.0f, 2.61f);
	m1=wb_robot_get_device("m1"); initServo(m1, 1000000.0f, 2.61f);
}

static void initCommunication() {
	radioEmitter = wb_robot_get_device("emitter");
	radioReceiver = wb_robot_get_device("receiver");
	wb_receiver_enable(radioReceiver, 8);
	int i;
	for(i=0;i<10;i++) {
		irEmitters[i] = wb_robot_get_device(irEmitterNames[i]);
		irReceivers[i] = wb_robot_get_device(irReceiversNames[i]);
		wb_receiver_enable(irReceivers[i], 8);
	}
}
static void initAccelerometer() {
	accelerometer = wb_robot_get_device("accelerometer");
	wb_accelerometer_enable(accelerometer, 1);
}

static void initConnectors() {
	int i;
	for(i=0;i<10;i++) {
		connectors[i] = wb_robot_get_device(connectorNames[i]);
		wb_connector_enable_presence(connectors[i], 10);
	}
}

static void initID() {
	const char* name = wb_robot_get_name();
	sscanf(name, "module_%d", &myID);
	printf("Got ID = %i from Name = %s\n",myID, name);
}

int roombotApi_isConnected(int connectorID) {
	printf("Warning: method 'roombotApi_isConnected' not implemented\n");
	//wb_connector_isLocked(connectors[connectorID]);
	return 0;
}

void roombotApi_connect(int connectorID) {
	wb_connector_lock(connectors[connectorID]);
}

void roombotApi_disconnect(int connectorID) {
	wb_connector_unlock(connectors[connectorID]);
}

int roombotApi_getConnectorPresence(int connectorID) {
	return wb_connector_get_presence(connectors[connectorID]);
}

double roombotApi_getServoPosition(int servoID) {
	return wb_servo_get_position(getServoFromID(servoID));
}

void roombotApi_setServoPosition(int servoID, double pos) {
	wb_servo_set_position(getServoFromID(servoID), pos);
}
double roombotApi_getAccelerometerValue(int axis) {
	return wb_accelerometer_get_values(accelerometer)[axis];
}

void roombotApi_sendMessage(char* msg, int length, int channel) {
	if(channel>=0 && channel<10) {
		if(roombotApi_getConnectorPresence(channel)) {
			wb_emitter_send(irEmitters[channel], msg, length);
		}
	}
	else if(channel==10) {
		wb_emitter_send(radioEmitter, msg, length);
	}
	else {
		printf("Warning: Roombot does not have a communication channel %i\n",channel);
	}

}

/**
 * Note: this method only sends to neighbors (not all in robot)
 */
void roombotApi_sendMessageToAll(char* msg, int length) {
	int i;
	for(i=0;i<10;i++) {
		roombotApi_sendMessage(msg, length, i);
	}
}

double roombotApi_getLocalTime() {
	return wb_robot_get_time();
}

long roombotApi_getLocalMsTime() {
	return (long)(1000*roombotApi_getLocalTime());
}

long roombotApi_getRandomSeed() {
	long seed = myID+roombotApi_getLocalMsTime()+time(NULL);
	printf("Module %i is using random seed = %li\n",myID,seed);
	return seed;
}

int roombotApi_getID() {
	return myID;
}

static int checkForMessages(WbDeviceTag device, int channel) {
	int msgCount=0;
	while (wb_receiver_get_queue_length(device) > 0) {
	  char *message = (char*) wb_receiver_get_data(device);
	  int  length = wb_receiver_get_data_size(device);

	  if(messageHandler!=NULL) {
		  messageHandler(message, length, channel);
		  //printf(" Strength = %f\n",wb_receiver_get_signal_strength(device));
		  //const double* dir = wb_receiver_get_emitter_direction(device);
		  //printf(" Dir = {%f, %f, %f}\n",dir[0],dir[1],dir[2]);
	  }
	  else {
		  printf("Warning: throwing away message, no handler defined\n");
	  }
	  wb_receiver_next_packet(device);
	  msgCount++;
	}
	return msgCount;
}

float roombotApi_getEnergyConsumed() {
	return energyConsumed;
}
static void updateEnergyConsumed() {
	int i;
	for(i=0;i<3;i++) {
		float torque = wb_servo_get_motor_force_feedback(getServoFromID(i));
		float angle = wb_servo_get_position(getServoFromID(i));
		energyConsumed += torque*(angle-oldAngle[i]);
		oldAngle[i] = angle;
	}
}

int roombotApi_checkForEvents() {
	int i, eventCount=0;
	updateEnergyConsumed();
	for(i=0;i<10;i++) {
		eventCount += checkForMessages(irReceivers[i], i);
	}
	eventCount += checkForMessages(radioReceiver, 10);
	return eventCount;
}


void roombotApi_init(void (*handler)(char*,char,char)) {
	messageHandler = handler;
	initServos();
    initCommunication();
    initConnectors();
    initID();
    initAccelerometer();
}

void roombotApi_runTestProgram() {
	printf("Running Roombot API Test Program...\n");
	wb_robot_init();
	roombotApi_setServoPosition(0, 1.5);
	roombotApi_setServoPosition(1, 1.5);
	roombotApi_setServoPosition(2, 1.5);
	do {
		printf("Servo 0 at pos %f\n", roombotApi_getServoPosition(0));
		printf("Servo 1 at pos %f\n", roombotApi_getServoPosition(1));
		printf("Servo 2 at pos %f\n", roombotApi_getServoPosition(2));
		char msg[6] = {'h','e','l','l','o','\0'};
		roombotApi_sendMessage(msg, sizeof(msg), 0);
		roombotApi_checkForEvents();
	} while (wb_robot_step(TIME_STEP) != -1);
	wb_robot_cleanup();
}

//to test api as single application
/*
static void handleMessage(char* msg, char length, char channel) {
	printf("Got message: %s of length %i on channel %i\n",msg, length, channel);
}
int main() {
	wb_robot_init();
	roombotApi_init(handleMessage);
	roombotApi_runTestProgram();
	return 0;
}*/


