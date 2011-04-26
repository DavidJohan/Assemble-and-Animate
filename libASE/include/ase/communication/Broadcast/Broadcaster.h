/**
\file Broadcaster.h

Broadcaster.h enables the modules to broadcast messages, each module in the robot will received the broadcasted message once.
Received messages must elsewhere be forwarded to the broadcast(.) function, which will make the decision
on whether or not to transmit the message further. This file is used by MCManager.h

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef BROADCASTER_H_
/**
 * Initializes the broadcaster
 */
void broadcaster_init();


/**
 * Broadcast the message. Note that this function will only transmit the
 * message if it is not already stored in a cache memory (not already send).
 */
int broadcaster_broadcast(char* message, char messageSize, char channel);

#define BROADCASTER_H_

#endif /* BROADCASTER_H_ */

