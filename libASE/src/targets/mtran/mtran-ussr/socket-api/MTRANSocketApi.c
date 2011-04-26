#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/targets/mtran.h>
#include <ase/utils/socket-tools/CommandSender.h>
#include <ase/utils/socket-tools/EventListner.h>
#include <ase/utils/socket-tools/SimpleSocket.h>

#define MAX_MSG_SIZE 600
#define STR_BUF_SIZE 500
#define IR_MSG_MAXSIZE 1000

static char mtranApi_msgReceiveBuffer[MAX_MSG_SIZE];
static char mtranApi_msgSendBuffer[MAX_MSG_SIZE];
static void (*mtranApi_msgHandler)(char*, char, char);

int mtranApi_wasSend() {
  return cs_wasSuccess();
}

char* mtranApi_getName(char* returnString) {
  return cs_sendCommand_string("getName",returnString);
}

int mtranApi_isRotating(int actuator) {
  char str[STR_BUF_SIZE]; sprintf(str, "isRotating %d", actuator);
  return cs_sendCommand_bool(str);
}

void mtranApi_rotateContinuous(int actuator,int vel)  {
  char str[STR_BUF_SIZE]; sprintf(str, "rotateContinuous %d %d", vel, actuator);
  cs_sendCommand_void(str);
}

void mtranApi_rotateTo(int actuator,float rad,int dir)  {
  char str[STR_BUF_SIZE];
  char* dirstr = dir ? "true" : "false";
  sprintf(str, "rotateClockwiseTo %f %d %s", rad, actuator, dirstr);
  cs_sendCommand_void(str);
}

void mtranApi_rotateToDegree(int actuator,float rad,int dir) {
  mtranApi_rotateTo(actuator, rad,dir);
}

void mtranApi_rotateDirToInDegrees(int actuator, int degrees,int dir) {
  mtranApi_rotateTo(actuator,degrees/180.0f,dir);
}
float mtranApi_getTime() {
  char str[STR_BUF_SIZE]; sprintf(str, "getTime");
  return cs_sendCommand_float(str);
}

long mtranApi_getMsTime() {
  float time = mtranApi_getTime();
  return (long)(time*1000.0);
}

long mtranApi_getRandomSeed() {
  char str[STR_BUF_SIZE]; sprintf(str, "getRandomSeed");
  return (long) cs_sendCommand_int(str);
}


float mtranApi_getAngularPosition(int actuator) {
  char str[STR_BUF_SIZE]; sprintf(str, "getEncoderPosition %d", actuator);
  return cs_sendCommand_float(str);
}

int mtranApi_getAngularPositionDegrees(int actuator) {
  //  char str[50]; sprintf(str, "getAngularPositionDegrees %d", actuator);
  // return cs_sendCommand_int(str);
  return mtranApi_getAngularPosition(actuator)*180;
}

void mtranApi_disconnectAll() {
  cs_sendCommand_void("disconnectAll");
}

void mtranApi_connectAll()  {
  cs_sendCommand_void("connectAll");
}

int mtranApi_canConnect(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "canConnect %d", connector);
  return cs_sendCommand_bool(str);
}

int mtranApi_canDisconnect(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "canDisconnect %d", connector);
  return cs_sendCommand_bool(str);
}

int mtranApi_isMale(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "isMale %d", connector);
  return cs_sendCommand_bool(str);
}

void mtranApi_connect(int connector)  {
  char str[STR_BUF_SIZE]; sprintf(str, "connect %d", connector);
  cs_sendCommand_void(str);
}

void mtranApi_disconnect(int connector)  {
  char str[STR_BUF_SIZE]; sprintf(str, "disconnect %d", connector);
  cs_sendCommand_void(str);
}

int mtranApi_isConnected(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "isConnected %d", connector);
  return cs_sendCommand_bool(str);
}

//returns int as boolean
int mtranApi_isDisconnected(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "isDisconnected %d", connector);
  return cs_sendCommand_bool(str);
}

void mtranApi_centerBrake(int actuator) {
  char str[STR_BUF_SIZE]; sprintf(str, "centerBrake %d", actuator);
  cs_sendCommand_void(str);
}
void mtranApi_centerStop(int actuator) {
  char str[STR_BUF_SIZE]; sprintf(str, "centerStop %d", actuator);
  cs_sendCommand_void(str);
}

//returns int as boolean
int mtranApi_isOtherConnectorNearby(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "isOtherConnectorNearby %d", connector);
  return cs_sendCommand_bool(str);
}

//returns int as boolean
int mtranApi_isObjectNearby(int connector) {
  char str[STR_BUF_SIZE]; sprintf(str, "isObjectNearby %d", connector);
  return cs_sendCommand_bool(str);
}

int mtranApi_getHardwareID() {
  return cs_sendCommand_int("getModuleID");
}

//short used to be byte
short mtranApi_getTiltX() {
  return cs_sendCommand_int("getTiltX");
}

//short used to be byte
short mtranApi_getTiltY() {
  return cs_sendCommand_int("getTiltY");
}
//short used to be byte
short mtranApi_getTiltZ() {
  return cs_sendCommand_int("getTiltZ");
}

void mtranApi_yield() {
  cs_sendCommand_void("yield");
}

void mtranApi_delay(long ms) {
  char str[STR_BUF_SIZE]; sprintf(str, "delay %li", ms);
  cs_sendCommand_void(str);
}

void mtranApi_lockActuators() {
  cs_sendCommand_void("lockActuators");
}

void charArrayToString(char* array, char* result, char arrayLength) {
  char charBuffer[10];
  int i,j,n,index = 0;
  for(i=0;i<arrayLength;i++) {
    n=sprintf(charBuffer, "%d",array[i]);
    for(j=0;j<strlen(charBuffer);j++) {
      result[index++] = charBuffer[j];
    }
    result[index++] = ',';
  }
  result[index] = '\0';
}

void stringToCharArray(char* string, char* result) {
  int index=0;
  char* token;
  for ( token = strtok(string,",");token != NULL; token = strtok(NULL, ",") ) {
    result[index++] = atoi(token);
  }
  result[index] = '\0';
}

//returns int as boolean
static char mtranApi_messageStr[IR_MSG_MAXSIZE];
static int useAck = 0;
int mtranApi_sendMessage(char* message, char messageSize, char connector) {
  charArrayToString(message, mtranApi_messageStr, messageSize);
  sprintf(mtranApi_msgSendBuffer, "sendMessage %s %d %d", mtranApi_messageStr, messageSize, connector);
  if(connector>=0&&connector<6) {
    if(useAck) {
      if(mtranApi_isOtherConnectorNearby(connector))  {
	return cs_sendCommand_int(mtranApi_msgSendBuffer);
      }
      return 0;
    }
    else {
      cs_sendCommand_int(mtranApi_msgSendBuffer); //send into blue air
      return 1;
    }
  }
  return 0;
}

int mtranApi_sendMessageToAll(char* message, char messageSize) {
  int i, count=0;
  for(i=0;i<6;i++) {
    count += mtranApi_sendMessage(message, messageSize, i);
  }
  return count;
}

int mtranApi_sendMessageToAllExcept(char* message, char messageSize, char except) {
  int i, count=0;
  for(i=0;i<6;i++) {
    if(i!=except) {
      count += mtranApi_sendMessage(message, messageSize, i);
    }
  }
  return count;
}

void mtranApi_handleMessage(char* eventType, char* messageData) {
  char* message = strtok(messageData," "); //handleMessage string
  int messageSize = atoi(strtok(NULL, " "));
  int channel = atoi(strtok(NULL, " "));
  stringToCharArray(message, mtranApi_msgReceiveBuffer);
  mtranApi_msgHandler(mtranApi_msgReceiveBuffer, messageSize, channel);
}

void mtranApi_setup() {
}

void mtranApi_ussrSetup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char)) {
  /*Initialize Message Handler*/
  mtranApi_msgHandler = msgHandler;

  /*Initialize command sender*/
  cs_init(portRC, host);
  cs_sendCommand_void("setup"); //initializes simulator

  /*Initialize event listener*/
  el_init(portEvent, host);
  el_installEvent("handleMessage", mtranApi_handleMessage);
  el_startEventListen();
}
