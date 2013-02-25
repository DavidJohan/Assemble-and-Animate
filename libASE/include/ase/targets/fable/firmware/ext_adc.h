/*
 * adc.h
 *
 * Created: 21/09/2012 12:01:55
 *  Author: Walteruzzu
 */ 


#ifndef EXT_ADC_H_
#define EXT_ADC_H_

#define ADC_ADDR 0x90

#define ADC_CHANNEL_0 0x00
#define ADC_CHANNEL_1 0x04
#define ADC_CHANNEL_2 0x01
#define ADC_CHANNEL_3 0x05
#define ADC_CHANNEL_4 0x02
#define ADC_CHANNEL_5 0x06
#define ADC_CHANNEL_6 0x03
#define ADC_CHANNEL_7 0x07

#include "i2c.h"

void ext_adc_init();

uint16_t ext_adc_read(uint8_t channel_mask);




#endif /* ADC_H_ */