#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <avr/iom2561.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ase/targets/fable/firmware/dxl_hal.h"
#include "ase/targets/fable/firmware/macro.h"


#define DYNAMYXEL_EN_PORT PORTE
#define DYNAMIXEL_EN_TX 2
#define DYNAMIXEL_EN_RX 3

#define MAXNUM_DXLBUFF	256
// Porting
//#define DIR_TXD 	PORTE &= ~0x08, PORTE |= 0x04
#define DIR_TXD CLEAR(DYNAMYXEL_EN_PORT,DYNAMIXEL_EN_RX), SET(DYNAMYXEL_EN_PORT,DYNAMIXEL_EN_TX)
//#define DIR_RXD 	PORTE &= ~0x04, PORTE |= 0x08
#define DIR_RXD CLEAR(DYNAMYXEL_EN_PORT,DYNAMIXEL_EN_TX), SET(DYNAMYXEL_EN_PORT,DYNAMIXEL_EN_RX)

volatile unsigned char gbDxlBuffer[MAXNUM_DXLBUFF] = {0};
volatile unsigned char gbDxlBufferHead = 0;
volatile unsigned char gbDxlBufferTail = 0;
volatile double gfByteTransTime_us;
volatile unsigned int gwCountNum;
volatile unsigned int gwTimeoutCountNum;
volatile unsigned int gwReturnDelayCountNum;

int dxl_hal_get_qstate(void);
void dxl_hal_put_queue( unsigned char data );
unsigned char dxl_hal_get_queue(void);


int dxl_hal_open(int devIndex, float baudrate)
{
	// Opening device
	// devIndex: Device index
	// baudrate: Real baudrate (ex> 115200, 57600, 38400...)
	// Return: 0(Failed), 1(Succeed)
	
	unsigned short Divisor;

	// dynamixel communication using UART0

	
	// set UART register A
	//Bit 7: USART Receive Complete
	//Bit 6: USART Transmit Complete
	//Bit 5: USART Data Resigter Empty 
	//Bit 4: Frame Error
	//Bit 3: Data OverRun
	//Bit 2: Parity Error
	//Bit 1: Double The USART Transmission Speed
	//Bit 0: Multi-Processor Communication Mode
	UCSR0A = 0b01000010;
	
	// set UART register B
	// bit7: enable rx interrupt
    // bit6: enable tx interrupt
    // bit4: enable rx
    // bit3: enable tx
    // bit2: set sendding size(0 = 8bit)
	UCSR0B = 0b10011000;
	
	// set UART register C
	// bit6: communication mode (1 = synchronize, 0 = asynchronize)
    // bit5,bit4: parity bit(00 = no parity) 
    // bit3: stop bit(0 = stop bit 1, 1 = stop bit 2)
    // bit2,bit1: data size(11 = 8bit)
	UCSR0C = 0b00000110;
	
	// Set baudrate
	//Divisor = (unsigned short)(2000000.0 / baudrate) - 1;
	//Divisor = (unsigned short)((F_CPU/8) / baudrate) - 1;
	Divisor = (unsigned short)(1000000.0 / baudrate) - 1;
	UBRR0H = (unsigned char)((Divisor & 0xFF00) >> 8);
	UBRR0L = (unsigned char)(Divisor & 0x00FF);

	gfByteTransTime_us = 1000000.0 / (double)baudrate * 12.0;
	gwReturnDelayCountNum = (unsigned int)(250.0 / gfByteTransTime_us);
	
	// initialize
	DIR_RXD;
	UDR0 = 0xFF;
	gbDxlBufferHead = 0;
	gbDxlBufferTail = 0;
	return 1;
}

void dxl_hal_close(void)
{
	// Closing device
}

void dxl_hal_clear(void)
{
	// Clear communication buffer
	gbDxlBufferHead = gbDxlBufferTail;
}

int dxl_hal_tx( unsigned char *pPacket, int numPacket )
{
	// Transmiting date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data transmitted. -1 is error.	
	int count;
	
	cli();
	DIR_TXD;
	for( count=0; count<numPacket; count++ )
	{
		while(!bit_is_set(UCSR0A,5));
		
		UCSR0A |= 0x40;
		UDR0 = pPacket[count];
	}
	while( !bit_is_set(UCSR0A,6) );
	DIR_RXD;
	sei();
	return count;
}

int dxl_hal_rx( unsigned char *pPacket, int numPacket )
{
	// Recieving date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data recieved. -1 is error.
	int count, numgetbyte;
	
	if( gbDxlBufferHead == gbDxlBufferTail )
		return 0;
	
	numgetbyte = dxl_hal_get_qstate();
	if( numgetbyte > numPacket )
		numgetbyte = numPacket;
	
	for( count=0; count<numgetbyte; count++ )
		pPacket[count] = dxl_hal_get_queue();
	
	return numgetbyte;
}

void dxl_hal_set_timeout( int NumRcvByte )
{
	// Start stop watch
	// NumRcvByte: number of recieving data(to calculate maximum waiting time)
	gwCountNum = 0;	
	gwTimeoutCountNum = (NumRcvByte + 20) + gwReturnDelayCountNum;
}

int dxl_hal_timeout(void)
{
	// Check timeout
	// Return: 0 is false, 1 is true(timeout occurred)
	gwCountNum++;
		
	if( gwCountNum > gwTimeoutCountNum )
	{
		return 1;
	}
	
	_delay_us(gfByteTransTime_us);
	return 0;
}

int dxl_hal_get_qstate(void)
{
	short NumByte;
	
	if( gbDxlBufferHead == gbDxlBufferTail )
		NumByte = 0;
	else if( gbDxlBufferHead < gbDxlBufferTail )
		NumByte = gbDxlBufferTail - gbDxlBufferHead;
	else
		NumByte = MAXNUM_DXLBUFF - (gbDxlBufferHead - gbDxlBufferTail);
	
	return (int)NumByte;
}

void dxl_hal_put_queue( unsigned char data )
{
	if( dxl_hal_get_qstate() == (MAXNUM_DXLBUFF-1) )
		return;
		
	gbDxlBuffer[gbDxlBufferTail] = data;

	if( gbDxlBufferTail == (MAXNUM_DXLBUFF-1) )
		gbDxlBufferTail = 0;
	else
		gbDxlBufferTail++;
}

unsigned char dxl_hal_get_queue(void)
{
	unsigned char data;
	
	if( gbDxlBufferHead == gbDxlBufferTail )
		return 0xff;
		
	data = gbDxlBuffer[gbDxlBufferHead];
		
	if( gbDxlBufferHead == (MAXNUM_DXLBUFF-1) )
		gbDxlBufferHead = 0;
	else
		gbDxlBufferHead++;
		
	return data;
}

SIGNAL(USART0_RX_vect)
{
	dxl_hal_put_queue( UDR0 );
}
