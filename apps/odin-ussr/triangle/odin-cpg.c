#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ase/OdinSocketApi.h>
#include <ase/control/controller.h>

char* host = "localhost";
int portRC=9900;
int portEvent=9901;

void parseCommandLineParameters(int argc, char *argv[]) {
	int i;
	for(i=1;i<argc;i++) {
		char * pch;
		pch = strtok (argv[i],"=");
		while (pch != NULL)
		{
			if(strcmp(pch,"-portRC")==0) {
				pch = strtok (NULL, "=");
				if(pch==NULL) {
					printf("-portRC set error\n");
					break;
				}
				else {
					portRC = atoi(pch);
					//printf("PortRC set to %i\n",portRC);
				}
				break;
			}
			if(strcmp(pch,"-portEvent")==0) {
				pch = strtok (NULL, "=");
				if(pch==NULL) {
					printf("-portEvent set error\n");
					break;
				}
				else {
					portEvent = atoi(pch);
					//printf("PortEvent set to %i\n",portEvent);
				}
				break;
			}
			if(strcmp(pch,"-host")==0) {
				pch = strtok (NULL, "=");
				if(pch==NULL) {
					printf("-host set error\n");
					break;
				}
				else {
					host = pch;
					//printf("Host set to %s\n",host);
				}
				break;
			}
			pch = strtok (NULL, "=");
		}
	}
}
int main(int argc, char *argv[])
{
	parseCommandLineParameters(argc, argv);
	odinApi_setup(portRC, portEvent, host, handleMessage);
	controller_start();
	printf("Controller terminated\n");
	return 0;
}
