/**
\file SimpleSocket.h

SimpleSocket.h provides simple socket interface as needed by CommandSender.h and EventListner.h to interface with the Unified Simulator for Self-Reconfigurable Robots (USSR).

\author David Johan Christensen, Modular Robotics Lab, University of Southern Denmark

\note This software is distributed under the BSD open-source license.
*/

#ifndef SIMPLESOCKET_H_

#define __USE_POSIX
#define __USE_GNU

void sprintf_take();
void sprintf_release();
/**
Connect to a socket, blocks until connection is made, terminates process is no connection is achieved.
\param host typically LOCALHOST
\param port any free socket port e.g. 1234
\return sockfd the id of the socket which was connected to.
*/
int ss_connect(char* host, int port);

/**
Receive data from a socket, blocks until data received (newline terminated).
\param buffer the buffer to put received data into.
\param sockfd the id of the socket to read from.
\return numbytes the number of bytes received.
*/
int ss_receive(char* buffer, int sockfd);

/**
Send data to a socket, blocks until data send. Data must be terminated with newline.
\param data the data to be send.
\param length the number of bytes to be send.
\param sockfd the id of the socket to send data to.
*/
void ss_send(char* data, int length, int sockfd);

/**
Close the socket.
\param sockfd the id of the socket to close.
*/
void ss_close(int sockfd);

#define SIMPLESOCKET_H_


#endif /* SIMPLESOCKET_H_ */
