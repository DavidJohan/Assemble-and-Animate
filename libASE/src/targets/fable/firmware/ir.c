/*
 * ir.c
 *
 * Created: 12/10/2012 14:55:45
 *  Author: Walteruzzu
 */ 
#include "ase/targets/fable/firmware/ir.h"
#include "ase/targets/fable/firmware/ext_uart.h"
#include <util/delay.h>



void ir_init(uint8_t uart){
	
		//clock to gpio
		i2c_write_byte(uart,0x20,0x80); //clock to gpio on usart 2
		i2c_write_byte(uart,0x18,1<<2); //gpio2 as output
		//reset ir transceiver
		i2c_write_byte(uart,0x19,0);
		_delay_ms(100);
		i2c_write_byte(uart,0x19,1<<2);
		_delay_ms(100);
		
		
		
}
