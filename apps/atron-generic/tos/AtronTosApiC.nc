/*
 * Copyright (c) 2007-2008 University of Southern Denmark
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 * - Neither the name of the the copyright holders nor the names of
 *   their contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
 * HOLDERS OR THEIR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * ATRON demo application, useful also as a template.
 *
 * @author Mirko Bordignon
 */
#include "controller.h"
#include "printf.h"
#include <ase/targets/atron/AtronApi.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>

#define UNKNOWN       0
#define CONNECTED     1
#define CONNECTING    2
#define DISCONNECTED  3
#define DISCONNECTING 4

module AtronTosApiC
{
  uses {
    interface Boot;
    interface Leds;
    interface Connector[uint8_t connector];
    interface CentralJoint;
    interface Packet;
    interface InfraredPacket;
    interface NeighborTable;
    interface Send;
    interface Receive;
    interface Receive as SerialReceive;
    interface Get<uint16_t> as getModuleIdOnFlash;
    interface LocalTime<TMilli>;

    interface Read<uint16_t> as Battery;
    interface Read<uint16_t> as readAccX;
    interface Read<uint16_t> as readAccXother; // the x axis on the other(north) hemisphere
    interface Read<uint16_t> as readAccY;
    interface Read<uint16_t> as readAccZ;

    interface Timer<TMilli>;
  }
}
implementation
{

  static int connectorState[8];
  static uint8_t readDegreesDoneFlag = 1;
  static uint16_t atronApi_degrees;
  static uint16_t batteryLevel;

  event void Boot.booted() {  }

  event void readAccX.readDone(error_t error, uint16_t val) {
  }

  event void readAccXother.readDone(error_t error, uint16_t val) {
  }

  event void readAccY.readDone(error_t error, uint16_t val) {
  }

  event void readAccZ.readDone(error_t error, uint16_t val) {
  }

  event void Battery.readDone(error_t error, uint16_t val) {
	  batteryLevel = val;
  }


  void setStartState(module_state_t state) @C() @spontaneous()  {
    int i;
    for(i=0;i<8;i++) {
      connectorState[i] = UNKNOWN;
    }
  }

  void callMe() @C() @spontaneous() {
    call Leds.set(255);
  }
  void atronApi_setup()  @C() @spontaneous() {
    // call Timer.startPeriodic(5);
     call Timer.startPeriodic(100);
  }
  void atronApi_yield() @C() @spontaneous() {
    //dummy for ussr compatability
  }

  int atronApi_getLeds() @C() @spontaneous() {
    return call Leds.get();
  }

  void atronApi_setLeds(int val) @C() @spontaneous() {
    call Leds.set(val);
  }

  long atronApi_getMsTime() @C() @spontaneous()  {
    return call LocalTime.get();
  }

  int atronApi_getBatteryLevel() @C() @spontaneous() {
	  //float scaledLevel = (float)batteryLevel*0.0156402737f from (64/20)*(5/1023), see Mirkos note in BatteryVoltageP.nc
	  int scaledLevel = batteryLevel*16;
  	  return scaledLevel;
  }

  float atronApi_getTime() @C() @spontaneous()  {
    uint64_t msTime =  call LocalTime.get();
    return ((float) msTime)/1000.0;
  }

  uint8_t channel2bitpattern(uint8_t channel) {
    if(channel==0) return CONNECTOR_0;
    else if(channel==1) return CONNECTOR_1;
    else if(channel==2) return CONNECTOR_2;
    else if(channel==3) return CONNECTOR_3;
    else if(channel==4) return CONNECTOR_4;
    else if(channel==5) return CONNECTOR_5;
    else if(channel==6) return CONNECTOR_6;
    else if(channel==7) return CONNECTOR_7;
    else if(channel==255) return ALL_CONNECTORS;
    return 0;
  }
  uint8_t bitpattern2channel(uint8_t bitpattern) {
    if(bitpattern==CONNECTOR_0) return 0;
    else if(bitpattern==CONNECTOR_1) return 1;
    else if(bitpattern==CONNECTOR_2) return 2;
    else if(bitpattern==CONNECTOR_3) return 3;
    else if(bitpattern==CONNECTOR_4) return 4;
    else if(bitpattern==CONNECTOR_5) return 5;
    else if(bitpattern==CONNECTOR_6) return 6;
    else if(bitpattern==CONNECTOR_7) return 7;
    else if(bitpattern==ALL_CONNECTORS) return 255;
    return 99;
  }

  bool debugSend = FALSE;
  message_t pkt;
  bool busy = FALSE;
  int atronApi_sendMsg(char* message, char messageSize, char connector)   {
    int i;
    if (!busy) {
      char* msg = (char*)(call Packet.getPayload(&pkt, messageSize));
      if (msg == NULL) {
	return 0;
      }
      for(i=0;i<messageSize;i++) {
	msg[i] = message[i];
      }
      call InfraredPacket.setConnector(&pkt, connector);
      if (call Send.send(&pkt, messageSize) == SUCCESS) {
        busy = TRUE;
	return 1;
      }
      return 0;
    }
    return 0;
  }

  int atronApi_sendMessage(char* message, char messageSize, char connector) @C() @spontaneous()  {
    int channel = channel2bitpattern(connector);
    return atronApi_sendMsg(message, messageSize, channel);
  }

  int atronApi_sendMessageToAll(char* message, char messageSize) @C() @spontaneous()  {
    return atronApi_sendMsg(message, messageSize, ALL_CONNECTORS);
  }

  int atronApi_sendMessageToAllExcept(char* message, char messageSize, char except) @C() @spontaneous()  {
    int channels = ALL_CONNECTORS & ~channel2bitpattern(except);
    return atronApi_sendMsg(message, messageSize, channels);
  }

  int atronApi_isOtherConnectorNearby(int channel) @C() @spontaneous()  {
    /*if(((call NeighborTable.read())&&channel2bitpattern(channel))!=0) {
      return 1;
    }
    else {
      return 0;
      }*/
    return 0;
  }

  void atronApi_ussrSetup(int portRC, int portEvent, char* host, void (*msgHandler)(char*, char, char)) @C() @spontaneous(){}

  char* atronApi_getName(char* returnString) @C() @spontaneous(){return NULL;}

  long atronApi_getRandomSeed() @C() @spontaneous() {
    return 123; //TODO make something that reads a sensor value
  }

  uint8_t isRotatingFlag=0;
  int atronApi_isRotating() @C() @spontaneous(){
    return isRotatingFlag;
  }

  int atronApi_getJointPosition() @C() @spontaneous(){return 0;}
  void atronApi_rotate(int dir) @C() @spontaneous(){
    //isRotatingFlag = 1;
    //
  }
  void atronApi_rotateDegrees(int degrees) @C() @spontaneous(){
    // isRotatingFlag = 1;
  }

  int atronApi_getDistanceToFromDir(int to, int from, bool dir) {
    int dist;
    if(dir) {
      dist = (to-from);
    }
    else {
      dist = (from-to);
    }
    if(dist<0) {
      dist += 360;
    }
    else if(dist>360) {
      dist -= 360;
    }
    return dist;
  }


  void atronApi_rotateToDegreeInDegrees(int degrees) @C() @spontaneous() {
    /* int dis1, dis2;
    int degreesScaled = (int) (1.2f*degrees);
    isRotatingFlag = 1;
    dis1 =  atronApi_getDistanceToFromDir(degrees, atronApi_degrees, TRUE);
    dis2 =  atronApi_getDistanceToFromDir(degrees, atronApi_degrees, FALSE);
    if(dis1<dis2) {
      call CentralJoint.rotateToDegreesShortestPath(degreesScaled, 150);
      //call CentralJoint.rotateToDegrees(degreesScaled, 150, TRUE);
    }
    else {
      call CentralJoint.rotateToDegreesShortestPath(degreesScaled, 150);
      // call CentralJoint.rotateToDegrees(degreesScaled, 150, FALSE);
    }*/

    int degreesScaled = (int) (1.2f*degrees);
    isRotatingFlag = 1;
    call CentralJoint.rotateToDegreesShortestPath(degreesScaled, 150);
  }

  void atronApi_rotateToViaInDegrees(int goalDegree, int viaDegree) @C() @spontaneous() {
    int dis1, dis2;
    int degreesScaled = (int) (1.2f*goalDegree);
    isRotatingFlag = 1;
    dis1 =  atronApi_getDistanceToFromDir(viaDegree, atronApi_degrees, TRUE);
    dis2 =  atronApi_getDistanceToFromDir(viaDegree, atronApi_degrees, FALSE);
    if(dis1<dis2) {
      call CentralJoint.rotateToDegrees(degreesScaled, 150, TRUE);
    }
    else {
      call CentralJoint.rotateToDegrees(degreesScaled, 150, FALSE);
    }
  }

  void atronApi_rotateDirToInDegrees(int to, int dir) @C() @spontaneous() {
    int toScaled = (int) (1.2f*to);
    isRotatingFlag = 1;
    call CentralJoint.rotateToDegrees(toScaled,150,dir);
  }

  void atronApi_rotateToDegree(float rad) @C() @spontaneous(){
    atronApi_rotateToDegreeInDegrees((360.0f*rad)/(6.28f));
  }
  float atronApi_getAngularPosition() @C() @spontaneous(){
    return 0.0f;
  }
  void atronApi_home() @C() @spontaneous(){
   atronApi_rotateToDegreeInDegrees(0);
  }

  event void CentralJoint.rotationCompleted(error_t error, uint16_t degrees, bool dir) {
    isRotatingFlag = 0;
  }

   int atronApi_getAngularPositionDegrees() @C() @spontaneous(){
    // readDegreesDoneFlag = 0;
    //call CentralJoint.readDegrees();
    // while(!readDegreesDoneFlag);  //TODO setup timer to keep read degrees - how often?
    return atronApi_degrees;
  }
  event void CentralJoint.readDegreesDone(error_t error, uint16_t deg) {
    readDegreesDoneFlag=1;
    atronApi_degrees = (int)(0.8333f*deg);
  }

  void atronApi_rotateContinuous(float dir) @C() @spontaneous(){
    //TODO change simulator and here to rotate with a torque instead.
    if(dir==-1.0) call CentralJoint.rotate(150, FALSE);
    if(dir== 1.0) call CentralJoint.rotate(150, TRUE);
    isRotatingFlag=1;
  }
  void atronApi_centerBrake() @C() @spontaneous(){
     isRotatingFlag=0;
     call CentralJoint.stop(TRUE);
  }
  void atronApi_centerStop() @C() @spontaneous(){
     isRotatingFlag=0;
     call CentralJoint.stop(FALSE);
  }


  void atronApi_disconnectAll() @C() @spontaneous(){
    call Connector.retract[CONNECTOR_0]();
    call Connector.retract[CONNECTOR_2]();
    call Connector.retract[CONNECTOR_4]();
    call Connector.retract[CONNECTOR_6]();
  }
  void atronApi_connectAll() @C() @spontaneous(){
    call Connector.extend[CONNECTOR_0]();
    call Connector.extend[CONNECTOR_2]();
    call Connector.extend[CONNECTOR_4]();
    call Connector.extend[CONNECTOR_6]();
  }
  int atronApi_canConnect(int i) @C() @spontaneous(){
    //?
    return 0;
  }
  int atronApi_canDisconnect(int i) @C() @spontaneous(){
    //?
    return 0;
  }
  int atronApi_isMale(int i) @C() @spontaneous(){
    return i%2==0;
  }

  void atronApi_connect(int i) @C() @spontaneous(){
    uint8_t connector = channel2bitpattern((uint8_t)i);
    connectorState[i]=CONNECTING;
    call Connector.extend[connector]();
  }
  void atronApi_disconnect(int i) @C() @spontaneous(){
    uint8_t connector = channel2bitpattern(i);
    connectorState[i]=DISCONNECTING;
    call Connector.retract[connector]();
  }
  int atronApi_isConnected(int i) @C() @spontaneous(){
     return connectorState[i]==CONNECTED;
  }
  int atronApi_isDisconnected(int i) @C() @spontaneous(){
    return connectorState[i]==DISCONNECTED;
  }
  int atronApi_getHardwareID() @C() @spontaneous(){
  	return call getModuleIdOnFlash.get();
    //return TOS_NODE_ID;
  }

  event void Connector.extendDone[uint8_t connector]( error_t error ) {
    int i = bitpattern2channel(connector);
    connectorState[i]=CONNECTED;
  }

  event void Connector.retractDone[uint8_t connector]( error_t error ) {
    int i = bitpattern2channel(connector);
    connectorState[i]=DISCONNECTED;
  }




  int atronApi_isObjectNearby(int connector) @C() @spontaneous(){return 0;}
  short atronApi_getTiltX() @C() @spontaneous(){return 0;}
  short atronApi_getTiltY() @C() @spontaneous(){return 0;}
  short atronApi_getTiltZ() @C() @spontaneous(){return 0;}
  void atronApi_delay(long ms) @C() @spontaneous(){}

  event message_t* Receive.receive(message_t* msg, void* payload, uint8_t len) {
    uint8_t connector = call InfraredPacket.getConnector(msg);
    ase_handleMessage((char*) payload, (char) len, (char) bitpattern2channel(connector));
    return msg;
  }

  event message_t* SerialReceive.receive(message_t* msg, void* payload, uint8_t len) {
	  ase_handleMessage((char*) payload, (char)len, 8);
	  return msg;
  }

  event void Send.sendDone(message_t* msg, error_t error) {
    if (&pkt == msg) {
      busy = FALSE;
    }
    if(error!=SUCCESS) { }
   }

  /**
   * To keep local sensry values sort of up to date
   */
  event void Timer.fired() {
    //  if( readDegreesDoneFlag == 0) return;
    //readDegreesDoneFlag = 1;
	 call Battery.read();
    call CentralJoint.readDegrees();
  }
}

