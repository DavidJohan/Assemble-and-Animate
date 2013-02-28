#ifndef __STDDEFS_h
#define __STDDEFS_h

#include <util/delay.h>

//Define bool
#ifndef bool
#define bool	uint8_t
#endif
#define true	1
#define false	0

#ifndef NULL
	#define NULL ((void *)0)
#endif

//Define simple bit, byte, word commands
#ifndef outb
	#define	outb(addr, data)	addr = (data)
#endif
#ifndef inb
	#define	inb(addr)			(addr)
#endif
#ifndef outw
	#define	outw(addr, data)	addr = (data)
#endif
#ifndef inw
	#define	inw(addr)			(addr)
#endif
#ifndef BV
	#define BV(bit)				(1<<(bit))
#endif
#ifndef cbi
	#define cbi(reg,bit)		reg &= ~(BV(bit))
#endif
#ifndef sbi
	#define sbi(reg,bit)		reg |= (BV(bit))
#endif
#ifndef cli
	#define cli()				__asm__ __volatile__ ("cli" ::)
#endif
#ifndef sei
	#define sei()				__asm__ __volatile__ ("sei" ::)
#endif
#ifndef nop
	#define nop()				__asm__ __volatile__ ("nop" ::)
#endif

//Define max and minimum of commmon types
#define MAX_uint8_t		255
#define MAX_uint16_t	65535
#define MAX_uint32_t	4294967295

// maximum values that can be held
// by signed data types (8,16,32bits)
#define MIN_int8_t		-128
#define MAX_int8_t		127
#define MIN_int16_t		-32768
#define MAX_int16_t		32767
#define MIN_int32_t		-2147483648
#define MAX_int32_t		2147483647

//Min, max and abs commands
#define MIN(a,b)		((a<b)?(a):(b))
#define MAX(a,b)		((a>b)?(a):(b))
#define ABS(x)			((x>0)?(x):(-x))

//Simple delays
static __inline__ void delay_us(uint16_t us) {
	for(uint16_t i=0;i<us/10;i++) {
		_delay_us(10);
	}
}

static __inline__ void delay_ms(uint16_t ms) {
	for(uint16_t i=0;i<ms;i++) {
		_delay_ms(1);
	}
}

static __inline__ void delay(uint16_t s) {
	for(uint16_t i=0;i<s;i++) {
		delay_ms(1000);
	}
}

#endif
