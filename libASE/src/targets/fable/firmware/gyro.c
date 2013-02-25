/*
 * gyro.c
 *
 * Created: 18/09/2012 18:09:16
 *  Author: Walteruzzu
 */ 
#include "ase/targets/fable/firmware/gyro.h"

void gyro_write_byte(uint8_t reg, uint8_t value){
	i2c_write_byte(GYRO_ADDR,reg,value);
}

int8_t gyro_read_byte(uint8_t reg){
	/*i2c_send_start();
	i2c_send_addr(GYRO_ADDR,TW_WRITE);
	i2c_send_data( reg);
	i2c_send_start();
	i2c_send_addr(GYRO_ADDR,TW_READ);
	int8_t data= i2c_receive_data(0);
	i2c_send_stop();
	return data;	*/
	return i2c_read_byte(GYRO_ADDR,reg);
}

void gyro_set_mode(const uint8_t mode){
	//gyro_cfg(0x20,mode);
	uint8_t prev = i2c_read_byte(GYRO_ADDR,0x20);
	i2c_write_byte(GYRO_ADDR,0x20, prev | mode );
}

int16_t gyro_read_axis(uint8_t axis){
	/*i2c_send_start();
	i2c_send_addr(GYRO_ADDR,TW_WRITE);
	i2c_send_data( axis | (1<<7));
	i2c_send_start();
	i2c_send_addr(GYRO_ADDR,TW_READ);
	int datal,datah;
	datal=i2c_receive_data(1);
	datah=i2c_receive_data(0);
	i2c_send_stop();
	return ((datah<<8) + datal);//&0x7ff;*/
	
	return i2c_read_word(GYRO_ADDR, (axis | (1<<7)));
}

void gyro_read_all(int16_t*x,int16_t*y,int16_t*z){
	
}
