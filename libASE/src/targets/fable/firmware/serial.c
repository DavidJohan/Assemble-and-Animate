#include <avr/io.h>
#include <avr/iom2561.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "ase/targets/fable/firmware/serial.h"

#ifndef F_CPU
#define F_CPU 8000000UL
#endif


#define __STDIO_FDEVOPEN_COMPAT_12 //for backward compatibility

#define MAXNUM_SERIALBUFF	128
#define DEFAULT_BAUDRATE	34 // 57132(57600)bps

#define DIR_RXD 	PORTE &= ~0x04, PORTE |= 0x08

volatile unsigned char gbSerialBuffer[MAXNUM_SERIALBUFF] = {0};
volatile unsigned char gbSerialBufferHead = 0;
volatile unsigned char gbSerialBufferTail = 0;
static FILE *device;

static volatile serial_rx_callback rx_callback = NULL;

//void serial_put_queue( unsigned char data );
//unsigned char serial_get_queue(void);
int std_putchar(char c);
int std_getchar(void);

void serial_set_rx_callback(const serial_rx_callback callback){
	rx_callback=callback;
}


void serial_initialize(long ubrr)
{
	// Serial communication using UART1
	//	int baud = (unsigned short)(2000000.0 / ubrr) - 1;
	int baud = (unsigned long)((F_CPU/8) / (ubrr)) - 1;
	// set UART register A
	//Bit 7: USART Receive Complete
	//Bit 6: USART Transmit Complete
	//Bit 5: USART Data Resigter Empty 
	//Bit 4: Frame Error
	//Bit 3: Data OverRun
	//Bit 2: Parity Error
	//Bit 1: Double The USART Transmission Speed
	//Bit 0: Multi-Processor Communication Mode
	UCSR1A = 0b00000010;
	//	UCSR1A = 0b01000000;

	// set UART register B
	// bit7: enable rx interrupt
	// bit6: enable tx interrupt
	// bit4: enable rx
	// bit3: enable tx
	// bit2: set sendding size(0 = 8bit)
	UCSR1B = 0b10011000;

	// set UART register C
	// bit6: communication mode (1 = synchronize, 0 = asynchronize)
	// bit5,bit4: parity bit(00 = no parity)
	// bit3: stop bit(0 = stop bit 1, 1 = stop bit 2)
	// bit2,bit1: data size(11 = 8bit)
	UCSR1C = 0b00000110;

	// initialize
	UDR1 = 0xFF;
	gbSerialBufferHead = 0;
	gbSerialBufferTail = 0;

	UBRR1H = (unsigned char)(baud>>8);

	UBRR1L = (unsigned char)(baud & 0xFF);
	//UBRR1H=0;
	//UBRR1L=103;

	DIR_RXD;
	device = fdevopen( std_putchar, std_getchar );
}

void serial_write( unsigned char *pData, int numbyte )
{
	int count;

	for( count=0; count<numbyte; count++ )
	{
		while(!bit_is_set(UCSR1A,5));
		UDR1 = pData[count];
	}
}

unsigned char serial_read( unsigned char *pData, int numbyte )
{
	int count, numgetbyte;

	if( gbSerialBufferHead == gbSerialBufferTail )
		return 0;

	numgetbyte = serial_get_qstate();
	if( numgetbyte > numbyte )
		numgetbyte = numbyte;

	for( count=0; count<numgetbyte; count++ )
		pData[count] = serial_get_queue();

	return numgetbyte;
}

int serial_get_qstate(void)
{
	short NumByte;

	if( gbSerialBufferHead == gbSerialBufferTail )
		NumByte = 0;
	else if( gbSerialBufferHead < gbSerialBufferTail )
		NumByte = gbSerialBufferTail - gbSerialBufferHead;
	else
		NumByte = MAXNUM_SERIALBUFF - (gbSerialBufferHead - gbSerialBufferTail);

	return (int)NumByte;
}

void serial_put_queue( unsigned char data )
{
	if( serial_get_qstate() == (MAXNUM_SERIALBUFF-1) )
		return;

	gbSerialBuffer[gbSerialBufferTail] = data;

	if( gbSerialBufferTail == (MAXNUM_SERIALBUFF-1) )
		gbSerialBufferTail = 0;
	else
		gbSerialBufferTail++;
}

unsigned char serial_get_queue(void)
{
	unsigned char data;

	if( gbSerialBufferHead == gbSerialBufferTail )
		return 0x00;

	data = gbSerialBuffer[gbSerialBufferHead];

	if( gbSerialBufferHead == (MAXNUM_SERIALBUFF-1) )
		gbSerialBufferHead = 0;
	else
		gbSerialBufferHead++;

	return data;
}

SIGNAL(USART1_RX_vect)
{
	serial_put_queue( UDR1 );
	if (rx_callback != NULL)
		rx_callback();
}



int std_putchar(char c)
{
	char tx[2];

	if( c == '\n' )
	{
		tx[0] = '\r';
		tx[1] = '\n';
		serial_write( (unsigned char*)tx, 2 );
	}
	else
	{
		tx[0] = c;
		serial_write( (unsigned char*)tx, 1 );
	}

	return 0;
}

int std_getchar(void)
{
	char rx;

	while( serial_get_qstate() == 0 );
	rx = serial_get_queue();

	if( rx == '\r' )
		rx = '\n';

	return rx;
}


