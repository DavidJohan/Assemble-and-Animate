/*
 * File:         client.c
 * Date:         October, 2003
 * Description:  A simple client program to connect to the TCP/IP server
 * Author:       Darren Smith
 * Modifications:
 *
 * Copyright (c) 2006 Cyberbotics - www.cyberbotics.com
 */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ase/utils/socket-tools/CommandSender.h>

#define SOCKET_PORT 10020
#define SOCKET_SERVER "127.0.0.1"   /* local host */
static int fd, rc;
/**
  * B: read software version
  * D: set speed
  * G: set position counter
  * H: read position
  * L: change LED state
  * N: read proximity sensors
  * O: read ambient light sensors
 **/
int kheperaApi_setup() {
	cs_init(SOCKET_PORT, SOCKET_SERVER);
	cs_sendCommandCount(0);
	return 1;
}

int kheperaApi_readSoftwareVersion(uint16_t* sw_version) {
	char returnString[255];
	cs_sendCommand_string("B\n", returnString);
	sscanf(returnString, "b,%d,%d", (int*)&sw_version[0], (int*)&sw_version[1]);
	return 1;
}

int kheperaApi_setSpeed(int16_t left_speed, int16_t right_speed) {
	char buffer[20];
	char returnString[255];
	sprintf(buffer, "D,%d,%d\n", left_speed,right_speed);
	cs_sendCommand_string(buffer, returnString);
	if(returnString[0]!='d') {
		return 0;
	}
	return 1;
}

int kheperaApi_setPositionCounter(int16_t left, int16_t right) {
	char buffer[20];
	char returnString[255];
	sprintf(buffer, "G,%d,%d\n", left, right);
	cs_sendCommand_string(buffer, returnString);
	if(returnString[0]!='g') {
		return 0;
	}
	return 1;
}
int kheperaApi_readPosition(int16_t* position) {
	char returnString[255];
	cs_sendCommand_string("H\n", returnString);
	sscanf(returnString, "h,%d,%d", (int*)&position[0], (int*)&position[1]);
	return 1;
}

int kheperaApi_changeLedState(uint8_t led_number, uint8_t led_action) {
	char buffer[20];
	char returnString[255];
	sprintf(buffer, "L,%hd,%hd\n", led_number, led_action);
	cs_sendCommand_string(buffer, returnString);
	if(returnString[0]!='g') {
		return 0;
	}
	return 1;
}
int kheperaApi_readProximitySensors(uint16_t* values) {
	char returnString[255];
	int d[8], i;
	cs_sendCommand_string("N\n", returnString);
	returnString[100] = '\0';
	sscanf(returnString, "n,%d,%d,%d,%d,%d,%d,%d,%d", &d[0],&d[1],&d[2],&d[3],&d[4],&d[5],&d[6],&d[7]);
	for(i=0;i<8;i++) {
		values[i] = (int16_t) d[i];
	}
	return 1;
}
int kheperaApi_readAmbientLightSensors(uint16_t* values) {
	char returnString[255];
	int d[8], i;
	cs_sendCommand_string("O\n", returnString);
	sscanf(returnString, "o,%d,%d,%d,%d,%d,%d,%d,%d", &d[0],&d[1],&d[2],&d[3],&d[4],&d[5],&d[6],&d[7]);
	for(i=0;i<8;i++) {
		values[i] = (int16_t) d[i];
	}
	return 1;
}

int kheperaApi_exit() {
	cs_sendCommand_void("exit\n");
	return 1;
}


int kheperaApi_setup_old()
{
    struct sockaddr_in address;
    struct hostent *server;


    /* create the socket */
    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        printf("cannot create socket\n");
        return -1;
    }

    /* fill in the socket address */
    memset(&address, 0, sizeof(struct sockaddr_in));
    address.sin_family = AF_INET;
    address.sin_port = htons(SOCKET_PORT);
    server = gethostbyname(SOCKET_SERVER);

    if (server) {
        memcpy((char *) &address.sin_addr.s_addr, (char *) server->h_addr,
               server->h_length);
    } else {
        printf("cannot resolve server name: %s\n", SOCKET_SERVER);
        return -1;
    }

    /* connect to the server */
    rc = connect(fd, (struct sockaddr *) &address, sizeof(struct sockaddr));
    if (rc == -1) {
        printf("cannot connect to the server\n");
        return -1;
    }
    return 1;
}
/* int n;
	char buffer[256];
	for (;;) {
		printf("Enter command: ");
		scanf("%255s", buffer);
		n = strlen(buffer);
		buffer[n++] = '\n';
		buffer[n] = '\0';
		n = send(fd, buffer, n, 0);

		if (strncmp(buffer, "exit", 4) == 0) {
			break;
		}

		n = recv(fd, buffer, 256, 0);
		buffer[n] = '\0';
		printf("Answer is: %s", buffer);
	}
 */
int kheperaApi_readSoftwareVersion_old(uint16_t* sw_version) {
	/*char buffer[50];
	int n=0;
	buffer[n++] = 'B';
	buffer[n++] = '\n';
	buffer[n] = '\0';
	n = send(fd, buffer, n, 0);
	if (strncmp(buffer, "exit", 4) == 0) {
		//break;
		return 0;
	}
	n = recv(fd, buffer, 50, 0);
	buffer[n] = '\0';

	int version0, version1;
	sscanf(buffer, "b,%d,%d", &version0, &version1);
	sw_version[0] = version0;
	sw_version[1] = version1;*/
	return 1;
}

