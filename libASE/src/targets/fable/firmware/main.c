/*
 * fable2.c
 *
 * Created: 24/09/2012 21:22:09
 *  Author: Walteruzzu
 */ 
#ifndef F_CPU
	#define F_CPU 8000000UL
#endif


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "serial.h"
#include "dynamixel.h"
#include "gyro.h"
#include "motor.h"
#include "macro.h"
#include "ext_uart.h"
#include "accel.h"


uint8_t addr[4]={EXT_UART_0,EXT_UART_1,EXT_UART_2,EXT_UART_3};
static int device=0;
void serial_receive_data(void){
	int prova;
	
	uint8_t val;
	unsigned char data;
	//uint8_t addr[4];
	//addr[0]=0xd8;
	//addr[1]=0xb8;
	//addr[2]=0x58;
	//addr[3]=0x38;
	
	serial_read(&data,1);
	switch (data)
	{
	case '1':
	case '2':
	case '3':
	case '4':
		device = atoi(&data) -1 ;
		printf("Using USART%d, Slave addr %#X\n",device,addr[device]);
	break;
	case 't':
		i2c_write_byte(addr[device],0x00,0x21);
		printf("sent byte 0x21 to USART %d\n",device );
	break;
	case 'r':
		if(i2c_read_byte(addr[device],0x12)){
			val=i2c_read_byte(addr[device],0);
			printf("read %#x\n",val);
		}
		else
		printf("no values to read on USART %d\n",device);			
	break;
	case 's':{
		uint8_t buff[37];
		i2c_seq_read(addr[device],1,buff,37);//read all internal register
		int i;
		for (i=0;i<37;i++){
			printf("data on addr %#x = %#X\n",i+1,buff[i]);//print all internal register 
		}
	break;
	}
	case 'l':{
		uint8_t curr=i2c_read_byte(addr[device],0x0a);
		i2c_write_byte(addr[device],0x0a,TOGGLE(curr,5));
		printf("loopback on USART %d now set to %d\n",device,(GET(curr,5)>>5));
	break;
	}	
	
	case 'g':{
		printf("Gyro data X:%#x, Y:%#x, Z:%#x \n",
			gyro_read_axis(GYRO_OUT_X),
			gyro_read_axis(GYRO_OUT_Y),
			gyro_read_axis(GYRO_OUT_Z));
		break;
	}
	case 'a':{
		
		printf("ACCEL data X:%#x, Y:%#x, Z:%#x \n",
		accel_read(ACCEL_X),
		accel_read(ACCEL_Y),
		accel_read(ACCEL_Z));
		break;
	}
	case 'm':{
		static volatile uint16_t move;
		uint8_t ids[2];
		ids[0]=3;
		ids[1]=11;
		uint16_t pos[2];
		pos[0]=0+move;
		pos[1]=1023-move;
		//motor_move(ids[0],pos[0],0);
		cli();
		motor_sync_move(2,ids,pos,0);
		sei();
		move=(move+32)%1024;
		printf("position set to %d\n",/*(unsigned int)(posh<<8)+posl*/dxl_read_word(11,GOAL_POSITION_L));
		break;
	}
	case 'p':{
		static uint8_t reset=0;
		reset^=1;
		printf("IR Device reset %s\n",reset?"ON":"OFF");
		if(reset){
			i2c_write_byte(addr[2],0x19,0);
			i2c_write_byte(addr[3],0x19,0);
		}
		else {
		i2c_write_byte(addr[2],0x19,1<<2);
		i2c_write_byte(addr[3],0x19,1<<2);	
		}
		
	break;
	}	
	default:
		printf("INVALID CHOICE  %c\n",data);
		printmenu();

	}
}

void printmenu(){
	printf("	*************SAMPLE APP************\n\\
	________External UART:\n \\
	t: transmit data,\n \\
	r: receive data,\n 		\\
	s: current status,\n 	\\
	l: toggle loopback,\n 	\\
	1...4: change target device (current is USART %d)\n\\
	________Dynamixel:\n 	\\
	m:move motors,\n	\\
	________Gyro:\n 	\\
	g: read gyro data,\n	\\
	________Externadl ADC\n 	\\
	d: read external adc on channel 2\n	\\
	________Accelerometer \n 	\\
	a: read accelerometer	\\
	\n\n", device);
}

void ext_uart_receive(void){
	printf("interrrupt triggered, globalirq= %#x",i2c_read_byte(0xd8,0x1f));
	uint8_t calldev=0,i=0;
	
	//************'DIRTY' vs. 'ELEGANT'(& MORE DIFFICULT TO UNDERSTAND) ????
	//switch(i2c_read_byte(0xd8,0x1f)){//DIRTY
		switch(0x0F & (~(i2c_read_byte(0xd8,0x1f)))){//'ELEGANT'
		
		case 1<<0: //ORIGINAL BYTE= 0000 1110  beacame in elegant fashion -> 1111 0001 -> 0000 0001
			//14:	//dirty?
			calldev=0;
			break;
		case 1<<1: //ORIGINAL BYTE= 0000 1101
			//13:
			calldev=1;
			break;
		case 1<<2: //ORIGINAL BYTE= 0000 1011
			//11:
			calldev=2;
			break;
		case 1<<3:	//ORIGINAL BYTE= 0000 0111
			//7:
			calldev=3;
			break;
	}
	if(GET(i2c_read_byte(addr[calldev],0x02),3))//check the device triggered the input (should also clear flag)
		for(i=0;i<8;i++)
			printf("asynch receive in usart %x of %x\n",calldev,i2c_read_byte(addr[calldev],0));//read and print data
	else
		printf("boooH!!!\n");
		//printf("interrupt triggered by usart %d, ISR= %x", calldev, i2c_read_byte(addr[calldev],0x02));
}

int main(void)
{
	DDRA=0xFF;

	serial_initialize(57600);
	serial_set_rx_callback(&serial_receive_data);
	
	accel_init();
	
	i2c_init(100);// init i2c 100 khz
	//ext_adc_init();//only on new board
	ext_uart_init();
	ext_uart_enable_int_rx(2,&ext_uart_receive);

	ir_init(EXT_UART_2);
	ir_init(EXT_UART_3);
	
	gyro_set_mode(GYRO_MODE_ON);// turn on gyroscope
				
	dxl_initialize(0,207); //init dinamixel, baud= 57600 (to check)
	sei();//enable interrupts
	
	printf("External UART %s\n",i2c_read_byte(0xd8,0x1c)==1?"OK":"NOT OK");//check communication works
	printmenu();//show menu

	while(1) //main loooooooop: just blinks a led
    { 
		_delay_ms(200);
		PORTA^=1;//for the new board
		TOGGLE(PORTA,4);//for the old board
    }
}

	
	
	/*
	i2c_write_byte(0xd8,0x1e,0x09); //enable internal clock (works???)
	i2c_write_byte(0x58,0x20,0x80); //clock to gpio on usart 2
	i2c_write_byte(addr[3],0x20,0x80);//clock to gpio on usart 3
	
	
	
	//i2c_write_byte(addr[2],0x23,1);//timer value=1
		i2c_write_byte(addr[2],0x18,1<<2); //gpio2 as output
		//i2c_write_byte(addr[3],0x23,1);//timer value=1
		i2c_write_byte(addr[3],0x18,1<<2); //gpio2 as output
		//reset ir transceiver 
		i2c_write_byte(addr[2],0x19,0);
		i2c_write_byte(addr[3],0x19,0);
		_delay_ms(100);
		i2c_write_byte(addr[2],0x19,1<<2);
		i2c_write_byte(addr[3],0x19,1<<2);
		_delay_ms(100);
		
		i2c_write_byte(addr[2],0x0b,0x03);//set data word 8 bits
		
		//use irda encoding??
		//i2c_write_byte(addr[2],0x0e,3);
		//i2c_write_byte(addr[3],0x0e,3);
		
		int i=0;
		for(i=0;i<4;i++){
			i2c_write_byte(addr[i],0x09,1<<7);//enable external interrupt on IRQ
			i2c_write_byte(addr[i],0x10,1<<4);//set tx trigger level  to 8 byte
			i2c_write_byte(addr[i],0x01,1<<3);//enable interrupt
		}
	*/	