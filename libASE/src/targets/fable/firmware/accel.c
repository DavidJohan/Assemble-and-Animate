/*
 * accelerometer.c
 *
 * Created: 12/10/2012 11:55:58
 *  Author: Walteruzzu
 */
#include "ase/targets/fable/firmware/accel.h"

void accel_init(){
	
	ADCSRA= _BV(ADEN) | _BV(ADIF)  | _BV(ADPS2) |_BV(ADPS1) |_BV(ADPS0) ;  //Enable AD,Clean IF,ADPS2:0 = 111-> 1/128div
}

uint16_t accel_read(uint8_t axis){
	ADMUX = axis;					//Select ADC channel
	SET(ADCSRA,ADIF);				//Clear ADC Interrupt Flag
	SET(ADCSRA,ADSC);				//Start Conversion
	while( !(ADCSRA & (1 << ADIF)) ); 	//wait for conversion to finish
	return ADC;
}

void accelerometer_read_all(uint16_t* data){
	data[0]=accel_read(ACCEL_X);
	data[1]=accel_read(ACCEL_Y);
	data[2]=accel_read(ACCEL_Z);
}
