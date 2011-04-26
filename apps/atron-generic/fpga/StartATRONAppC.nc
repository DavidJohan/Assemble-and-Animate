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

configuration StartATRONAppC{}
implementation {
  components MainC, AtronTosApiC as api2c, StartATRONC as app, AtronApiC as api,ActiveMessageAddressC as AMaddr;
  
  MainC.Boot <- app;

  api2c.DebugLeds -> api;
  api2c.Connector -> api;
  api2c.CentralJoint -> api;
  api2c.Packet -> api;
  api2c.InfraredPacket -> api;
  api2c.Send -> api;
  api2c.Receive -> api;
  api2c.LocalTime -> api;
  api2c.ActiveMessageAddress -> AMaddr;

  components NeighborTableC;
  api2c.NeighborTable -> NeighborTableC;

  components SerialActiveMessageC as AM;
  api2c.SerialReceive -> AM.Receive[201];
  
  components new TimerMilliC() as timer;
  api2c.Timer -> timer;
  
  components new TimerMilliC() as timer2;
  app.Timer -> timer2;
  app.DebugLeds -> api;
 

//Xbee comminication
  components XbeeActiveMessageC as ActiveMessageC;
  /* the AM type is 200 for modular commander */
  components new XbeeAMSenderC(201) as AMSenderC;
  components new XbeeAMReceiverC(201) as AMReceiverC;
  api2c.XbeePacket -> AMSenderC;
  api2c.XbeeAMPacket -> AMSenderC;
  api2c.XbeeAMControl -> ActiveMessageC;
  api2c.XbeeAMSend -> AMSenderC;
  api2c.XbeeReceive -> AMReceiverC;
  app.XbeeAMControl -> ActiveMessageC;

  
//components RS485AcksC as ack, DebugLedsC as leds;
  //ack.DebugLeds -> leds;
}

