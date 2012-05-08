/**
\file CommandSender.h

CommandSender.h provides together with  EventListner.h an generic socket based interface to the Unified Simulator for Self-Reconfigurable Robots (USSR). String formatted as function calls are send to the simulator for execution (through reflection). For example "SendMessage 123,32,10, 3 5" will trigger the SendMessage method in USSR with the appropiate parameters: the message ({123,32,10}), the message length (3) and the channel(5).

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/


#ifndef COMMANDSENDER_H_
#define __USE_POSIX
#define __USE_GNU

/**
Initialize the commandSender with a free socket port (e.g. 1234) and a host (typically LOCALHOST)
*/
void cs_init(int port, char* host);

/**
 * Set if command counter is also send in the message (for khepera-webots compatability)
 */
void cs_sendCommandCount(int sendCommandCount);

/**
Generic method for sending a command to USSR the return parameter is returned as a string in returnParam
*/
int cs_sendCommand(char* command, char* returnParam);

/**
If last command was send successfully
*/
int cs_wasSuccess();

/**
Send a command to USSR returns the return parameter as a string
*/
char* cs_sendCommand_string(char* command, char* returnString);

/**
Send a command to USSR returns void
*/
void cs_sendCommand_void(char* command);

/**
Send a command to USSR returns the return parameter as an int
*/
int cs_sendCommand_int(char* command);

/**
Send a command to USSR returns the return parameter as a boolean int
*/
int cs_sendCommand_bool(char* command);

/**
Send a command to USSR returns the return parameter as a float
*/
float cs_sendCommand_float(char* command);

#define COMMANDSENDER_H_

#endif /* COMMANDSENDER_H_ */
