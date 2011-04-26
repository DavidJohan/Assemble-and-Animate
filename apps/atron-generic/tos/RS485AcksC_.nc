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
 * - Neither the name of the the copyright holders nor the names of their
 *   contributors may be used to endorse or promote products derived
 *   from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDERS OR THEIR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * RS485 ACK layer
 *
 * @author Mirko Bordignon
 */

#include <Serial.h>
#include "RS485.h"

module RS485AcksC {
  provides {
    interface Send[uart_id_t];
    interface Receive[uart_id_t];
    interface GetNow<uint8_t> as TxSeqno;
    interface SetNow<uint8_t> as RxSeqno;
  }
  uses {
    interface Send as SubSend[uart_id_t];
    interface Receive as SubReceive[uart_id_t];
    interface Timer<TMilli> as ackTimeout;
    interface DebugLeds;
  }
}
implementation {

  norace uint8_t txSeqno;
  norace uint8_t rxSeqno;
  uint8_t lastSeenRxSeqno;

  bool busy = FALSE;
  bool awaitingAck = FALSE;
  bool ackToSend = FALSE;
  bool outstandingPkt = FALSE;

  uart_id_t outstandingId;
  message_t* outstandingMsg;
  uint8_t outstandingLen;
  error_t signalError;

  task void sendAck() {
    if(!busy) {
      /* as we're sending an empty message we can pass no real buffer,
         so we don't allocate and waste precious memory */
      if (call SubSend.send[TOS_SERIAL_RS485_ACK_ID](0, 0) == SUCCESS) {
        busy = TRUE;
	ackToSend = FALSE;/*we enqueued the ACK*/
      }
      else
	post sendAck();	
    }
    else
      post sendAck();
  }

  task void resend() {
    /*note that, contrary to what happens when the somebody calls Send.send, we do not
      increment the txSeqno and instead we send the old one. In this way the receiver
      can detect duplicate packets, in case he did not lose our message but it was us
      losing his ACK*/
    if(!busy) {
      if (call SubSend.send[outstandingId](outstandingMsg, outstandingLen) == SUCCESS) {
	outstandingPkt = TRUE;
	busy = TRUE;
      }
      else
	post resend();	
    }
    else
      post resend();    
  }

  task void signalSendDone() {
    signal Send.sendDone[outstandingId](outstandingMsg, signalError);
  }
  
  /* GetNow command */
  async command uint8_t TxSeqno.getNow() {
    return txSeqno;
  }

  /* SetNow command */
  async command error_t RxSeqno.setNow(uint8_t val) {
    rxSeqno = val;
   return SUCCESS;
 }

  /* Send commands */
  command error_t Send.send[uart_id_t id](message_t* msg, uint8_t len) {
    /* 
       We wait on those flags before letting other components send through rs485: 
       busy -> the send state of rs485 itseld
       ackToSend -> we have to send an Ack, and this takes precedence
    */
    if(!busy && !ackToSend && !awaitingAck) {
      /*important NOT to use TOS_SERIAL_RS485_ACK_ID as a seqno*/
      if(++txSeqno == TOS_SERIAL_RS485_ACK_ID)
	txSeqno = 1;
      if (call SubSend.send[id](msg, len) == SUCCESS) {
	outstandingPkt = TRUE;
	outstandingLen = len;
	busy = TRUE;
	return SUCCESS;
      }
      else {
	return FAIL;
      }
    }
    else
      return EBUSY;
  }
 
  command error_t Send.cancel[uart_id_t id](message_t* msg) {
    return call SubSend.cancel[id](msg);
  }
  
  command uint8_t Send.maxPayloadLength[uart_id_t id]() {
    return call SubSend.maxPayloadLength[id]();
  }
  
  command void* Send.getPayload[uart_id_t id](message_t* msg, uint8_t len) {
    return call SubSend.getPayload[id](msg, len);
  }

  /* SubSend events */
  event void SubSend.sendDone[uart_id_t id](message_t* msg, error_t error) {
    if(error != SUCCESS)
      call DebugLeds.led0On();
    busy = FALSE;
    /*we are done sending an actual packet, we start waiting for the ACK*/
    if(id != TOS_SERIAL_RS485_ACK_ID) {
      outstandingPkt = FALSE;
      awaitingAck = TRUE;
      outstandingId = id;
      outstandingMsg = msg;
      signalError = error;
      call ackTimeout.startOneShot(RS485_ACK_TIMEOUT);
    }
  }

  /* SubReceive events */
  event message_t* SubReceive.receive[uart_id_t id](message_t* msg, void* payload, uint8_t len) {
    if(id == TOS_SERIAL_RS485_ACK_ID) {
      /*this is an ACK for our last msg*/
      awaitingAck = FALSE;
      call ackTimeout.stop();
      post signalSendDone();
      return msg;
    }
    else {
      /*whatever we receive and is not an ACK needs to be ACKed,
	otherwise the other side will keep resending it*/
      ackToSend = TRUE;
      post sendAck();
      /*is it an old packet, retransmitted by the other
	hemisphere thinking that we didn't receive it?*/
      if(rxSeqno != lastSeenRxSeqno) {
	/*no, remember it and pass it upstairs*/
	lastSeenRxSeqno = rxSeqno;
	return signal Receive.receive[id](msg, payload, len);
      }
      else {
	/*yes, drop it*/
	return msg;
      }
    }
  }

  event void ackTimeout.fired() {
    /*the message got corrupted or there was a channel collision, resend*/
    post resend();
  }

  /* Defaults */
  default event message_t* Receive.receive[uart_id_t idxxx](message_t *msg,
                                                         void *payload,
                                                         uint8_t len){
    return msg;
  }
  default event void Send.sendDone[uart_id_t idxxx](message_t *msg, error_t error){
    return;
  }
}
