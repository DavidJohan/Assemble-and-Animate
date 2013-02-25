/*
 * adc.c
 *
 * Created: 21/09/2012 12:01:42
 *  Author: Walteruzzu
 */ 
#include <avr/io.h>

#include "ase/targets/fable/firmware/ext_adc.h"

void ext_adc_init(void){
	//ENABLE INTERNAL REFERENCE
	i2c_send_start();
	i2c_send_addr(ADC_ADDR,TW_WRITE);
	i2c_send_data(0x80);
	i2c_send_stop();
}

uint16_t ext_adc_read(uint8_t channel_mask){
	/*
	i2c_send_start();
	i2c_send_addr(ADC_ADDR,TW_WRITE);
	i2c_send_data(0x08 | channel_mask<<4 |0x0A);
	i2c_send_start();
	i2c_send_addr(ADC_ADDR,TW_READ);
	uint8_t datah= i2c_receive_data(1);
	uint8_t datal= i2c_receive_data(0);
	i2c_send_stop();
	return ((datah<<8) +  datal);
	*/
	uint16_t temp= i2c_read_word(ADC_ADDR,0x08 | channel_mask<<4 |0x0A);
	return  ( ( temp & 0xff ) << 8) | ( temp >> 8 ); //swap high and low bytes
}
