// Atmega2561 version
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <ase/targets/dynamixel/dynamixel-cm510/firmware/zgb_hal.h>

#define MAXNUM_ZGBBUFF	256

volatile unsigned char gbZgbBuffer[MAXNUM_ZGBBUFF] = {0};
volatile unsigned char gbZgbBufferHead = 0;
volatile unsigned char gbZgbBufferTail = 0;

int zgb_hal_get_qstate(void);
void zgb_hal_put_queue( unsigned char data );
unsigned char zgb_hal_get_queue(void);

int zgb_hal_open( int devIndex, float baudrate )
{
	// Opening device
	// devIndex: Device index
	// baudrate: Real baudrate (ex> 115200, 57600, 38400...)
	// Return: 0(Failed), 1(Succeed)
	
	unsigned short Divisor;

	// Serial communication using UART1
	
	// set UART register A
	//Bit 7: USART Receive Complete
	//Bit 6: USART Transmit Complete
	//Bit 5: USART Data Resigter Empty 
	//Bit 4: Frame Error
	//Bit 3: Data OverRun
	//Bit 2: Parity Error
	//Bit 1: Double The USART Transmission Speed
	//Bit 0: Multi-Processor Communication Mode
	UCSR1A = 0b01000010;
	
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
	
	// Set baudrate
	Divisor = (unsigned short)(2000000.0 / baudrate) - 1;
	UBRR0H = (unsigned char)((Divisor & 0xFF00) >> 8);
	UBRR0L = (unsigned char)(Divisor & 0x00FF);
	
	// initialize
	UDR1 = 0xFF;
	gbZgbBufferHead = 0;
	gbZgbBufferTail = 0;
	return 1;
}

void zgb_hal_close(void)
{
	// Closing device

}

int zgb_hal_tx( unsigned char *pPacket, int numPacket )
{
	// Transmiting date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data transmitted. -1 is error.
	int count;
	
	for( count=0; count<numPacket; count++ )
	{
		while(!bit_is_set(UCSR1A,5));
		UDR1 = pPacket[count];
	}
	
	return count;
}

int zgb_hal_rx( unsigned char *pPacket, int numPacket )
{
	// Recieving date
	// *pPacket: data array pointer
	// numPacket: number of data array
	// Return: number of data recieved. -1 is error.
	int count, numgetbyte;
	
	numgetbyte = zgb_hal_get_qstate();
	if( numgetbyte > numPacket )
		numgetbyte = numPacket;
	
	for( count=0; count<numgetbyte; count++ )
		pPacket[count] = zgb_hal_get_queue();
	
	return numgetbyte;
}

int zgb_hal_get_qstate(void)
{
	short NumByte;
	
	if( gbZgbBufferHead == gbZgbBufferTail )
		NumByte = 0;
	else if( gbZgbBufferHead < gbZgbBufferTail )
		NumByte = gbZgbBufferTail - gbZgbBufferHead;
	else
		NumByte = MAXNUM_ZGBBUFF - (gbZgbBufferHead - gbZgbBufferTail);
	
	return (int)NumByte;
}

void zgb_hal_put_queue( unsigned char data )
{
	if( zgb_hal_get_qstate() == (MAXNUM_ZGBBUFF-1) )
		return;
	
	gbZgbBuffer[gbZgbBufferTail] = data;

	if( gbZgbBufferTail == (MAXNUM_ZGBBUFF-1) )
		gbZgbBufferTail = 0;
	else
		gbZgbBufferTail++;
}

unsigned char zgb_hal_get_queue(void)
{
	unsigned char data;
	
	if( zgb_hal_get_qstate() == 0 )
		return 0xff;
		
	data = gbZgbBuffer[gbZgbBufferHead];
		
	if( gbZgbBufferHead == (MAXNUM_ZGBBUFF-1) )
		gbZgbBufferHead = 0;
	else
		gbZgbBufferHead++;
		
	return data;
}

SIGNAL(USART1_RX_vect)
{
	zgb_hal_put_queue( UDR1 );
}
