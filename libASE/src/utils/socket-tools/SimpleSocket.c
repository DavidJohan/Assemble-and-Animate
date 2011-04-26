#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define MAXDATASIZE 100 // max number of bytes we can get at once


int mutex = 0;
void sprintf_take() {
	while(mutex>0); // \todo make atomic
	mutex++;
}
void sprintf_release() {
	mutex--;
}

int ss_connect(char* host, int port) {
	int sockfd;
	struct hostent *he;
	struct sockaddr_in their_addr; // connector's address information

	if ((he=gethostbyname(host)) == NULL) {  // get the host info
	    printf("ERROR: unable to connect to %s on port %i (1) \n", host, port);
	    exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		printf("ERROR: unable to connect to %s on port %i (2) \n", host, port);
	    	exit(1);
	}
	their_addr.sin_family = AF_INET;    // host byte order
	their_addr.sin_port = htons(port);  // short, network byte order
	their_addr.sin_addr = *((struct in_addr *)he->h_addr);
	memset(their_addr.sin_zero, '\0', sizeof their_addr.sin_zero);
	if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof their_addr) == -1) {
	    printf("ERROR: unable to connect to %s on port %i (3) \n", host, port);
	    exit(1);
	}
	return sockfd;
}

int receiveing = 0;
int ss_receive(char* buffer, int sockfd) {
	int numbytes;
	if(receiveing) {
		//printf("rec (%i) race condition: %s\n",sockfd, buffer);
		//while(receiveing==1);
	}
	receiveing=1;
	if ((numbytes=recv(sockfd, buffer, MAXDATASIZE-1, 0)) == -1) {
	    printf("ERROR: In ss_receive(...) while receiveding data from socket\n");
	    exit(1);
	}
	/*if(strpbrk(buffer, "#")==NULL) {
		printf("received buffer is not terminated, must be incomplete %s\n",buffer);
	}*/
	//printf("Received: %s (%i) to %i\n",buffer, numbytes, sockfd);
	buffer[numbytes] = '\0';
	receiveing = 0;
	return numbytes;
}
int sending = 0;
void ss_send(char* data, int length, int sockfd) {
	if(sending) {
	//	printf("send (%i) race condition: %s\n",sockfd, data);
		//while(sending==1);
	}
	sending=1;
	//printf("Sending: %s (%i) to %i\n",data, length, sockfd);
	if (send(sockfd, data, length, 0) == -1) {
		printf("ERROR: In ss_send(...) while sending data to socket\n");
		exit(1);
	}
	sending =0;
}

void ss_close(int sockfd) {
	close(sockfd);
}
