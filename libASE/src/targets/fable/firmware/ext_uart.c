/*
 * ext_uart.c
 *
 * Created: 12/10/2012 14:17:51
 *  Author: Walteruzzu
 */ 
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <ase/targets/fable/firmware/ext_uart.h>
#include <ase/targets/fable/firmware/i2c.h>


static volatile ext_uart_int_callback int_callback=NULL;


void ext_uart_init(){
	//reset uart
#ifdef OLD
	SET(DDRB, 0); // OLD BOARD UART RESET as output
	CLEAR(PORTB,0); //RESET UART OLD BOARD
	_delay_ms(100);
	SET(PORTB,0); //remove RESET OLD board
#else
	CLEAR(DDRE,4);//PORTE4 as input
	CLEAR(PORTE,4);//disable pullup
	SET(DDRG,2);//new board UART RESET as output
	CLEAR(PORTG,2);//RESET UART new board
	_delay_ms(100);
	SET(PORTG,2);	 //	remove RESET UART new board
#endif

	while(!GET(PINE,4)){	//wait reset to finish
		printf("waiting ext uart reset\n");
	}

	for(int i=0;i<4;i++){
		i2c_write_byte(ext_uart_channels[i],0xb,0x7);//data word length 8 bits
		i2c_write_byte(ext_uart_channels[i],0x1b,1<<6);//disable clock
		i2c_write_byte(ext_uart_channels[i],0x1c,0x4);		//BRG divider (w/int clock -> 9600Bps)
															//			  (w/ext clock -> 57600Bps)
		i2c_write_byte(ext_uart_channels[i],0x1b,0);//re-enable clock
	}

	//clock to gpio
	//		i2c_write_byte(EXT_UART_0,0x1e,0x88); //clock to rts
	//		i2c_write_byte(EXT_UART_1,0x1e,0x88); //clock to rts
	//		i2c_write_byte(EXT_UART_2,0x1e,0x88); //clock to rts
	//		i2c_write_byte(EXT_UART_3,0x1e,0x88); //clock to rts
#ifdef OLD
	i2c_write_byte(EXT_UART_0,0x1e,0x09);//enable internal clock
#endif

}

void ext_uart_enable_int_rx(uint8_t level,const ext_uart_int_callback callback){

	int i=0;
	uint8_t addr[4]={EXT_UART_0,EXT_UART_1,EXT_UART_2,EXT_UART_3};
	for(i=0;i<4;i++){
		i2c_write_byte(addr[i],0x09,1<<7);//enable external interrupt on IRQ
		i2c_write_byte(addr[i],0x10,level<<4);//set tx trigger level  to (level * 8 bytes)
		i2c_write_byte(addr[i],0x01,1<<3);//enable interrupt
	}
	CLEAR(PORTE,4); //UART INT pin  AS INPUT
	CLEAR(DDRE,4);// UART INT pin as input
	//enable interrupt on pin change
	SET(EIMSK,4);
	EICRB=2; //falling edge. DA PROVARE ANCHE 3, riging edge
	int_callback=callback;//set callback

}

ISR(INT4_vect){
	if(int_callback!=NULL)
		int_callback();
}

void ext_uart_send(uint8_t addr, uint8_t *message, uint8_t length){
	for(int i=0;i<length;i++)
		i2c_write_byte(addr,0x00,message[i]);
}

void ext_uart_0_send(uint8_t byte){
	i2c_write_byte(EXT_UART_0,0,byte);
}

void ext_uart_1_send(uint8_t byte){
	i2c_write_byte(EXT_UART_1,0,byte);
}

void ext_uart_2_send(uint8_t byte){
	i2c_write_byte(EXT_UART_2,0,byte);
}

void ext_uart_3_send(uint8_t byte){
	i2c_write_byte(EXT_UART_3,0,byte);
}

uint8_t ext_uart_0_rx(void){
	return i2c_read_byte(EXT_UART_0,0);
}
uint8_t ext_uart_1_rx(void){
	return i2c_read_byte(EXT_UART_1,0);
}
uint8_t ext_uart_2_rx(void){
	return i2c_read_byte(EXT_UART_2,0);
}
uint8_t ext_uart_3_rx(void){
	return i2c_read_byte(EXT_UART_3,0);
}
uint8_t ext_uart_0_poll(void){
	return i2c_read_byte(EXT_UART_0,0x12);
}
uint8_t ext_uart_1_poll(void){
	return i2c_read_byte(EXT_UART_1,0x12);
}
uint8_t ext_uart_2_poll(void){
	return i2c_read_byte(EXT_UART_2,0x12);
}
uint8_t ext_uart_3_poll(void){
	return i2c_read_byte(EXT_UART_3,0x12);
}


