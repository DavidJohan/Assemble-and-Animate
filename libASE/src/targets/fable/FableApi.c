/*
 * FableApi.c
 *
 *  Created on: 09/nov/2012
 *      Author: walteruzz
 */
#include <avr/io.h>
#include <avr/iom2561.h>
#include <avr/interrupt.h>

#include "ase/targets/fable.h"
#include "ase/targets/fable/firmware/i2c.h"
#include "ase/targets/fable/firmware/serial.h"
#include "ase/targets/fable/firmware/macro.h"
#include "ase/targets/fable/firmware/ext_uart.h"
#include "ase/targets/fable/firmware/ir.h"
#include "ase/targets/fable/firmware/gyro.h"
#include "ase/targets/fable/firmware/accel.h"


void ext_uart_receive(void){

}
void Fable_init(){
		DDRA=0xFF;

		//serial_initialize(9600);
		serial_initialize(57600);

		//serial_set_rx_callback(&serial_receive_data);
		//Init Timer
//		TCCR0A = (1<<COM0A1)|(1<<COM0A0);  //Set OC0A on Compare Match
//		TCCR0B = (1<<FOC0A)|(1<<CS01)|(1<<CS00); //Force Output Compare A and 64 prescaler
//		OCR0A = 125;	//1ms according to KAVRCalc (8Mhz, 1msm, 64 prescaler, 0.0% error)
//		TIMSK0 = 1<<OCIE0A; 	// Enable Timer 0 Output Compare A Match Event Interrupt

		TCCR0A = (1<<WGM01);//Clear on compare |(1<<COM0A0);  //Set OC0A on Compare Match
		TCCR0B = (1<<CS01)|(1<<CS00); //64 prescaler
		OCR0A = 125;	//1ms -> 8M/2*64*1000
		TIMSK0 = 1<<OCIE0A; 	// Enable Timer 0 Output Compare A Match Event Interrupt

		accel_init();
		i2c_init(400);// init i2c 100 khz
		ext_uart_init();
		ir_init(EXT_UART_0);
		ir_init(EXT_UART_1);
		ir_init(EXT_UART_2);
		ir_init(EXT_UART_3);

		gyro_set_mode(GYRO_MODE_ON);

		sei();//enable interrupts
/*
		//ext_adc_init();//only on new board
		ext_uart_enable_int_rx(1,&ext_uart_receive);


		ir_init(EXT_UART_2);
		ir_init(EXT_UART_3);

		gyro_set_mode(GYRO_MODE_ON);// turn on gyroscope

		//dxl_initialize(0,207); //init dinamixel, baud= 57600 (to check)

		//printf("External UART %s\n",i2c_read_byte(0xd8,0x1c)==1?"OK":"NOT OK");//check communication works
*/
}



void Fable_led_set(){
	CLEAR(PORTA,0);
}

void Fable_led_clear(){
	SET(PORTA,0);
}

void Fable_led_toggle(){
	TOGGLE(PORTA,0);
}

unsigned long Fable_get_accel(unsigned long axis){
	return accel_read(axis);
}

