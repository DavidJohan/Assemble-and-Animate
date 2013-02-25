/*
 * i2c.h
 *
 * Created: 18/09/2012 11:03:27
 *  Author: Walteruzzu
 */ 
#include <stdio.h>
#include <avr/io.h>
#include <util/twi.h>

#ifndef I2C_H_
#define I2C_H_

#ifndef DEBUG 
	#define DEBUG 1
#endif

#define I2C_READ 1
#define I2C_WRITE 0


uint8_t i2c_send_start();
void i2c_send_stop();
uint8_t i2c_send_addr(uint8_t addr, uint8_t rw);
uint8_t i2c_send_data(uint8_t data);
uint8_t i2c_receive_data(uint8_t ack);


void i2c_init(int baud_in_KHz);

void i2c_write_byte(uint8_t dev, uint8_t addr, uint8_t data);
uint8_t i2c_read_byte(uint8_t dev, uint8_t addr);

void i2c_write_word(uint8_t dev, uint8_t addr, uint16_t data);
uint16_t i2c_read_word(uint8_t dev, uint8_t addr);

uint8_t i2c_seq_read(uint8_t dev, uint8_t start_addr, uint8_t*buff, uint8_t len);


#endif /* I2C_H_ */