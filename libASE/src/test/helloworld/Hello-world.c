#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ase/Hello-world.h>

/**
Prints "ASE: Hello World\n" ousing printf.
*/
void helloworld_printHello() {
	printf("ASE: Hello World\n");
}

void helloworld_sendHello() {

}

int helloworld_getTrue() {
	return 1;
}
void helloworld_setLedsHello() {

}

/**
Prints "ASE: Hello World\n" to buffer. 
\param returnBuffer Buffer must be at least 18 bytes long.
\return returnBuffer
*/
char* helloworld_getHello(char* returnBuffer) {
	sprintf(returnBuffer, "ASE: Hello World\n");
	return returnBuffer;
}
