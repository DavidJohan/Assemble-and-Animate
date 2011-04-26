/**
\file EventListner.h

EventListner.h provides together with  CommandSender.h an generic socket based interface to the Unified Simulator for Self-Reconfigurable Robots (USSR). Events triggered in USSR are send to this EventListner through a socket. For example an event could be "HandleMessage 123,32,10, 3 1" which means the the module in USSR has received a message ({123,32,10}) of length 3 on channel 1. EventListner.h provides an observer based interface for handlers to be notified of such events.

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef EVENTLISTNER_H_
/**
Initialize the event listner with a free socket port and a host (typically LOCALHOST)
*/
void el_init(int portEvent, char* host);

/**
Starts a pthread that will listen for events recieved from USSR
*/
void el_startEventListen();

/**
This function will signal an event of the provided type and given parameters to all installed handlers for the given typs.
*/
void el_signalEvent(char* eventType, char* eventParameters);

/**
This will remove an installed event handler
\todo Test this function
*/
void el_removeEvent(char* eventType, void (event_routine)(char*, char*));

/**
This function installs an handler for a given event type (e.g. "HandleMessage"). A function pointer must be provided to the handler the parameters given to the handler are eventType and eventParameters
*/
void el_installEvent(char* eventType, void (event_routine)(char*, char*));

#define EVENTLISTNER_H_

#endif /* EVENTLISTNER_H_ */
