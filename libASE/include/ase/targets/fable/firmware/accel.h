/*
 * accel.h
 *
 * Created: 12/10/2012 11:56:46
 *  Author: Walteruzzu
 */ 


#ifndef ACCEL_H_
#define ACCEL_H_

#include <avr/io.h>
#include "macro.h"

#define ACCEL_X 3 
#define ACCEL_Y 2
#define ACCEL_Z 1

void accel_init(void);

uint16_t accel_read(uint8_t axis);

void accel_read_all(uint16_t* data);

#endif /* ADC_H_ */
