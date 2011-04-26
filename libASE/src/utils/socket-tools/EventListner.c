#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ase/utils/socket-tools/SimpleSocket.h>
#include <ase/utils/socket-tools/EventListner.h>

#define MAX_EVENTS	10
#define	BUFFER_SIZE	100

char el_receiveBuffer[BUFFER_SIZE];
char el_replyBuffer[BUFFER_SIZE];
int el_sockfd;


typedef struct
{
	char* type;
	void (*handler)(char*, char*);
} EventHandler;

typedef struct
{
	int handlerCount;
	EventHandler handlers[MAX_EVENTS];
} EventHandlers;

EventHandlers eventHandlers;

void el_installEvent(char* eventType, void (event_routine)(char*, char*)) {
	if(eventHandlers.handlerCount<MAX_EVENTS) {
		eventHandlers.handlers[eventHandlers.handlerCount].type = eventType;
		eventHandlers.handlers[eventHandlers.handlerCount].handler = event_routine;
		eventHandlers.handlerCount++;
	}
	else {
		printf("ERROR: Insufficient space (max=%i) for number of events, please increase avalable space (MAX_EVENTS) \n",MAX_EVENTS);
	}
}

// \todo not tested
void el_removeEvent(char* eventType, void (event_routine)(char*, char*)) {
	int i;
	for(i=0;i<eventHandlers.handlerCount;i++) {
		if(strstr(eventType, eventHandlers.handlers[i].type)!=NULL && event_routine==eventHandlers.handlers[i].handler)
		{
			printf("Event: %s removed...\n",eventHandlers.handlers[i].type);
			eventHandlers.handlers[i].type = eventHandlers.handlers[eventHandlers.handlerCount].type;
			eventHandlers.handlers[i].handler = eventHandlers.handlers[eventHandlers.handlerCount].handler;
			eventHandlers.handlerCount--;
		}
	}
}

void el_signalEvent(char* eventType, char* eventParameters) {
	int i;
	for(i=0;i<eventHandlers.handlerCount;i++) {
		if(strstr(eventType, eventHandlers.handlers[i].type)!=NULL)
		{
			//printf("Event: %s signaled...\n",eventHandlers.handlers[i].type);
			eventHandlers.handlers[i].handler(eventType, eventParameters);
		}
	}
}

void *el_listenForEvents() {
	while(1)
	{
		memset(el_receiveBuffer, 0, BUFFER_SIZE);
		ss_receive(el_receiveBuffer, el_sockfd);
		//printf("el_rec: %s", el_receiveBuffer);
		if(strlen(el_receiveBuffer)!=0)  {
			char *p;
			int id = atoi(strtok_r(el_receiveBuffer," ", &p));
			char* eventType = strtok_r(NULL," ", &p);
			char* eventString = strtok_r(NULL,"\n", &p);
		//	printf("Got event %i %s %s\n",id, eventType,eventString);
			el_signalEvent(eventType, eventString);
			memset(el_replyBuffer, 0, BUFFER_SIZE);

			sprintf_take();
			sprintf(el_replyBuffer, "%i OK 0\r\n", id);
			sprintf_release();

			ss_send(el_replyBuffer, strlen(el_replyBuffer), el_sockfd);
		//	printf("el_send: %s", el_replyBuffer);
		}
		else {
			printf("ERROR: In event listner null package received\n");
			exit(0);
			//memset(el_replyBuffer, 0, BUFFER_SIZE);
			//sprintf(el_replyBuffer, "0 ERROR 0\r\n");
			//ss_send(el_replyBuffer, strlen(el_replyBuffer), el_sockfd);
		}
	}
	printf("ERROR: Event thread exited\n");
	pthread_exit(0);
}

void el_startEventListen() {
	pthread_t listenThread;
	pthread_create(&listenThread, NULL, el_listenForEvents, NULL);
}


void el_init(int port, char* host) {
	el_sockfd = ss_connect(host, port);
	eventHandlers.handlerCount = 0;
	el_startEventListen();
}
