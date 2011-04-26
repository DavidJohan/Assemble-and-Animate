/**
\file LPFPrintf.h

 LPFPrintf.h provides an interface for an controller, controlling an LEGO Power Functions programmer

\author David Johan Christensen, Center for Playware, Technical University of Denmark

\note This software is distributed under the BSD open-source license.
*/
#ifndef LPFPRINTF_H_
#include <stdbool.h>
#define lpf_printf(...)  {\
	char buf[100];\
	int length = sprintf(buf,__VA_ARGS__);\
	usbSendPrintBuffer(buf, length);\
}

bool writePrintBuffer(char* buf, int length);
int readPrintBuffer(char* buf, int maxLength);
void usbSendPrintBuffer(char* buf, int length);
#define LPFPRINTF_H_
#endif
