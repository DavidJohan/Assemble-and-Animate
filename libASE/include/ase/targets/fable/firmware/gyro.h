/*
 * gyro.h
 *
 * Created: 18/09/2012 18:05:09
 *  Author: Walteruzzu
 */ 


#ifndef GYRO_H_
#define GYRO_H_

#define GYRO_ADDR 0xD2

#define GYRO_MODE_ON 0x0F
#define GYRO_MODE_SLEEP 0x08
#define GYRO_MODE_OFF 0x00

#define GYRO_OUT_X 0x28
#define GYRO_OUT_Y 0x2A
#define GYRO_OUT_Z 0x2C

#include "i2c.h"

void gyro_set_mode(uint8_t mode);
int16_t gyro_read_axis(uint8_t axis);
void gyro_read_all(int16_t*x,int16_t*y,int16_t*z);

//void gyro_write_byte(uint8_t reg, uint8_t value);
//int8_t gyro_read_byte(uint8_t reg);


#endif /* GYRO_H_ */