/*
 * i2c.c
 *
 * Created: 18/09/2012 10:51:58
 *  Author: Walteruzzu
 */ 
//#ifndef F_CPU
	#define F_CPU 8000000UL
//endif
#include <avr/io.h>
#include <stdio.h>
#include <util/twi.h>
#include <string.h>
#include "ase/targets/fable/firmware/i2c.h"
#include "ase/targets/fable/firmware/macro.h"
#include <setjmp.h>

volatile uint8_t i2c_status;


uint8_t i2c_send_start(){
	TWCR = _BV(TWINT) | _BV(TWSTA) | _BV(TWEN); /* send start condition */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((i2c_status = TW_STATUS))
	{
		case TW_REP_START:		/* OK, but should not happen */
		case TW_START:
			return 0;

		case TW_MT_ARB_LOST:
			#if DEBUG
			printf("I2C:arbitration lost on start bit\n");
			#endif
			return TW_MT_ARB_LOST;
		default:
		return 1;		/* error: not in start condition */
		/* NB: do NOT send stop condition! */
	}
}

void i2c_send_stop() {
	TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);
}

uint8_t i2c_send_addr(uint8_t addr, uint8_t rw){
	TWDR = addr | rw;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	if(rw ==  TW_READ){
		switch ((i2c_status = TW_STATUS))
		{
			case TW_MR_SLA_ACK:
			return 0;
			//break;
	
			case TW_MR_SLA_NACK:	/* nack during select: device busy writing */
				#if DEBUG
					printf("I2C:got a nack sending READ address 0x%x\n",addr);
				#endif
				i2c_send_stop();
				return TW_MR_SLA_NACK;

			case TW_MR_ARB_LOST:	/* re-arbitrate */
				#if DEBUG
					printf("I2C:arbitration lost sending READ address\n");
				#endif
				return TW_MR_ARB_LOST;
	
			default:
				#if DEBUG
					printf("I2C:error sending READ address,  status=%d \n",i2c_status);
				#endif
				i2c_send_stop();		/* must send stop condition */
				return i2c_status;
		}
	}
	else if(rw==TW_WRITE){
	switch ((i2c_status = TW_STATUS))
	{
		case  TW_MT_SLA_ACK:
			return 0;
		//break;
		
		case  TW_MT_SLA_NACK:	/* nack during select: device busy writing */
			#if DEBUG
				printf("I2C:got a nack sending WRITE address 0x%x\n",addr);
			#endif
			i2c_send_stop(); /*release the bus*/
		//TW_STATUS=0;
			return TW_MT_SLA_NACK;

		case TW_MT_ARB_LOST:	/* re-arbitrate */
			#if DEBUG
				printf("I2C:arbitration lost sending WRITE address\n");
			#endif
			return TW_MT_ARB_LOST;
		
		default:
			#if DEBUG
				printf("I2C:error sending WRITE address, status=%#x \n",i2c_status);
			#endif
			i2c_send_stop();		/* must send stop condition */
			return i2c_status;
	}
	}
return 0;
}

uint8_t i2c_receive_data(uint8_t ack){
	if(ack)
		TWCR = _BV(TWINT) | _BV(TWEN) | _BV(TWEA);
	else
		TWCR = _BV(TWINT) | _BV(TWEN);
	while ((TWCR & _BV(TWINT)) == 0) ;
	return TWDR;	
}

uint8_t i2c_send_data(uint8_t data){
	TWDR = data;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
	while ((TWCR & _BV(TWINT)) == 0) ; /* wait for transmission */
	switch ((i2c_status = TW_STATUS))
	{
		case TW_MT_DATA_ACK:
			return 0;
		//break;

		case TW_MT_DATA_NACK:	/* nack during select: device busy writing */
			#if DEBUG
				printf("I2C: got a nack sending byte 0x%x\n",data);
			#endif
			i2c_send_stop();
			return TW_MT_DATA_NACK;

		case TW_MT_ARB_LOST:	/* re-arbitrate */
			#if DEBUG
				printf("I2C:arbitration lost sending byte 0x%x\n",data);
			#endif
			return TW_MT_ARB_LOST;
		default:
			#if DEBUG
				printf("I2C:error in sub addr status=%d\n",i2c_status);
			#endif
			i2c_send_stop();		/* must send stop condition */
			return i2c_status;
	}
	return 0;
}

void i2c_init(int baud_in_KHz){
	TWBR=0;
	unsigned long temp=(F_CPU / (2.0*(unsigned long)baud_in_KHz*1000.0)) - 8.0 ;
	//TWBR= 32;
	TWBR= (uint8_t) temp;
	TWSR=0;
	TWCR=0;
	SET(TWCR,TWINT);
	SET(TWCR,TWEN);
	CLEAR(TWCR,TWIE);
	
}

uint8_t i2c_read_byte(uint8_t dev, uint8_t addr){
	if(i2c_send_start()!=0) return 0;
	if(i2c_send_addr(dev,TW_WRITE)!=0) return 0;
	if(i2c_send_data(addr)!=0) return 0;
	if(i2c_send_start()!=0) return 0;
	if(i2c_send_addr(dev,TW_READ)!=0) return 0;
	uint8_t data= i2c_receive_data(0);
	i2c_send_stop();
	return data;
}	
void i2c_write_byte(uint8_t dev, uint8_t addr, uint8_t data){
	if(i2c_send_start()!=0) return;
	if(i2c_send_addr(dev,TW_WRITE)!=0) return;
	if(i2c_send_data(addr)!=0) return ;
	if(i2c_send_data(data)!=0) return;
	i2c_send_stop();
}

uint16_t i2c_read_word(uint8_t dev, uint8_t addr){
	if(i2c_send_start()!=0) return 0;
	if(i2c_send_addr(dev,TW_WRITE)!=0) return 0;
	if(i2c_send_data(addr)!=0) return 0;
	if(i2c_send_start()!=0) return 0;
	if(i2c_send_addr(dev,TW_READ)!=0) return 0;
	uint8_t datal= i2c_receive_data(1);
	uint8_t datah= i2c_receive_data(0);
	i2c_send_stop();
	return ((datah<<8) |  datal);
}

void i2c_write_word(uint8_t dev, uint8_t addr,uint16_t data){
	if(i2c_send_start()!=0) return;
	if(i2c_send_addr(dev,TW_WRITE)!=0) return;
	if(i2c_send_data(addr)!=0) return;
	if(i2c_send_data(data & 0xff)!=0) return;
	if(i2c_send_data(((data & 0xff00)>>8))!=0) return;
	i2c_send_stop();
}

void i2c_seq_read(uint8_t dev, uint8_t start_addr, uint8_t*buff, uint8_t len){
	int i=0;
	if(i2c_send_start()!=0) return;
	if(i2c_send_addr(dev,TW_WRITE)!=0) return;
	if(i2c_send_data(start_addr)!=0) return;
	if(i2c_send_start()!=0) return;
	if(i2c_send_addr(dev,TW_READ)!=0) return;
	for(i=0;i<len;i++)
		*buff++=i2c_receive_data((i<(len-1))?1:0); //send ack until last byte to read
	i2c_send_stop();
}
