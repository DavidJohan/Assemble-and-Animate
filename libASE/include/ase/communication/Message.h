/**
\file Message.h

Message.h

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark
\note This software is distributed under the BSD open-source license.
*/

#ifndef MESSAGE_H_
#define MESSAGE_H_

/**
 * Contains info about a communication message
 */
typedef struct {
	char* message;
	char messageSize;
	char channel;
} Msg_t;

/**
 * Contains the header info of a message to be send
 */
typedef struct {
	char msgType;
	short label;
} Message_t;



#endif /* MESSAGE_H_ */
