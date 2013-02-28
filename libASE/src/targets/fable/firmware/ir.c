/*
 * ir.c
 *
 * Created: 12/10/2012 14:55:45
 *  Author: Walteruzzu
 */ 
#include <ase/targets/fable/firmware/ir.h>
#include <ase/targets/fable/firmware/ext_uart.h>
#include <util/delay.h>



void ir_init(uint8_t uart){
#ifndef OLD
	 	 i2c_write_byte(uart,0x1e,0x88);
#else
	 	 i2c_write_byte(uart,0x20,0x80);

#endif

//		i2c_write_byte(uart,0x18,1<<2); //gpio as output
		i2c_write_byte(uart,0x18,(1<<2)|(1<<3)); //gpio 2 and 3 as output

		//reset mcp2122
		i2c_write_byte(uart,0x19,0);
		_delay_ms(100);
		i2c_write_byte(uart,0x19,1<<2);
		_delay_ms(100);
		
		
		
}
