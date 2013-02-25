/*
 * ext_uart.c
 *
 * Created: 12/10/2012 14:17:51
 *  Author: Walteruzzu
 */ 
 #include <util/delay.h>
 #include <avr/interrupt.h>
 #include "ase/targets/fable/firmware/ext_uart.h"
 #include "ase/targets/fable/firmware/i2c.h"
#include <avr/iom2561.h>
 
 
 static volatile ext_uart_int_callback int_callback=NULL;

 
 void ext_uart_init(){
	
	 //SET(DDRG,2);//new board UART RESET as output
	  SET(DDRB, 0); // OLD BOARD UART RESET as output
	 	 
	 //reset uart
	 
	 //CLEAR(PORTG,2);//RESET UART new board
	 CLEAR(PORTB,0); //RESET UART OLD BOARD
	 
	 _delay_ms(10);
	 
	 //SET(PORTG,2);	 //	remove RESET UART new board
	 SET(PORTB,0); //remove RESET OLD board
	 
	 while(GET(PORTE,4)){
		 int dummy=0; //DEBUG: PLACE A BREAKPOINT AND SEE IF GET HERE
	 }
	 
	// i2c_write_byte(EXT_UART_0,0x1e,0x09);//enable internal clock
	 
	
 }
 
 void ext_uart_send(uint8_t addr, char* message, uint8_t length){
	 for(int i=0;i<length;i++)
		 i2c_write_byte(ext_uart_channels[addr],0x00,message[i]);
 }

 void ext_uart_loop_off(uint8_t channel){
	 if(channel>3){
		 printf("Channel %d does not exist, must be within [0,3]\n",channel);
		 return;
	 }
	 uint8_t curr=i2c_read_byte(ext_uart_channels[channel],0x0a);
	 i2c_write_byte(ext_uart_channels[channel],0x0a,CLEAR(curr,5));
 }
 void ext_uart_loop_on(uint8_t channel){
	 if(channel>3){
	 		 printf("Channel %d does not exist, must be within [0,3]\n",channel);
	 		 return;
	 	 }
	 	 uint8_t curr=i2c_read_byte(ext_uart_channels[channel],0x0a);
	 	 i2c_write_byte(ext_uart_channels[channel],0x0a,SET(curr,5));
 }


 void ext_uart_enable_int_rx(uint8_t level,const ext_uart_int_callback callback){
	
	 int i=0;
	 uint8_t addr[4]={EXT_UART_0,EXT_UART_1,EXT_UART_2,EXT_UART_3};
	 for(i=0;i<4;i++){
		 i2c_write_byte(addr[i],0x09,1<<7);//enable external interrupt on IRQ
		 i2c_write_byte(addr[i],0x10,level<<4);//set tx trigger level  to 8 byte
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
 
 
