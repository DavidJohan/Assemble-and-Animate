#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ase/targets/AbstractModuleApi.h>
#include <ase/targets/dynamixel.h>
#include <ase/infrastructure/Scheduler/Scheduler.h>
#include <ase/infrastructure/EventManager/EventManager.h>
#include <ase/communication/Message.h>
#include <ase/targets/dynamixel/dynamixel-cm510/firmware/serial.h>

#include <avr/io.h>
#include <avr/iom2561.h>
#include <avr/interrupt.h>

void delay_ms(long delay_ms) {
	long startTime = dynamixelApi_getMsTime();
	while((startTime+delay_ms)>dynamixelApi_getMsTime());
}

void testLed() {
	while (1) {
		int i;
		for(i=0;i<=6;i++) {
			dynamixelApi_CM510_setLed(i);
			delay_ms(500);
		}
		for(i=0;i<=6;i++) {
			dynamixelApi_CM510_clearLed(i);
			delay_ms(500);
		}
	}
	/*dynamixelApi_CM510_setLed(0);
	dynamixelApi_CM510_setLed(1);
	dynamixelApi_CM510_setLed(2);
	while(1) {
		dynamixelApi_CM510_toggleLed(2);
		myDelay(50000);
	}*/
}

bool gotMicEvent = false;
void micEvent() {
	gotMicEvent = true;
}
void testMic() {
	dynamixelApi_CM510_setMicEventFunction(micEvent);
	while (1){
		if(gotMicEvent) {
			ase_printf("Mic Events %i!\n",dynamixelApi_CM510_getMicEventCount());
			int i;
			for(i=0;i<7;i++) dynamixelApi_CM510_clearLed(i);
			delay_ms(1000);
			gotMicEvent = false;
		}
		else {
			int i;
			for(i=0;i<7;i++) dynamixelApi_CM510_setLed(i);
		}
	}
}

void testIR() {
	while(1) {
		int i=0;
		for(i=0;i<6;i++) {
			int val = dynamixelApi_CM510_getADC(i);
			printf( "%d, ", val);
			delay_ms(250);
		}
		printf( "\n");
	}
}

void testSerial() {
	ase_printf( "\n\nSerial Comm. example for CM-510\n\n" );
	while (1) {
		unsigned char ReceivedData = getchar();
		ase_printf( "Echo: %d \n", ReceivedData );
	}
}


void testSerialReadLine() {
	unsigned char buffer[50];
	int bufferIndex = 0;
	while (1) {
		if(serial_get_qstate()>0) {
			//buffer[bufferIndex] = serial_get_queue();
			serial_read(&buffer[bufferIndex], 1);
			if(buffer[bufferIndex]=='\n'||buffer[bufferIndex]=='\r') {
				buffer[bufferIndex+1] = 0;
				ase_printf("Echo Line: %s \n", buffer);
				bufferIndex = 0;
			}
			else {
				bufferIndex++;
			}
		}
	}
}

void testBuzz() {
	while (1) {
		PORTB |= 0x20;
		delay_ms(1);
		PORTB &= ~0x20;
		delay_ms(1);
	}
	//PORTB &= ~0x20;
	//dynamixelApi_CM510_buzz();
}

void testButtons(){
	while(1) {
		int i;
		for(i=0;i<5;i++) {
			if(dynamixelApi_CM510_getButton(i)) {
				dynamixelApi_CM510_setLed(i);
			}
			else {
				dynamixelApi_CM510_clearLed(i);
			}
		}
	}
}
void flowerInit() {
	dynamixelApi_setup(1,NULL); delay_ms(250);
	dynamixelApi_connect(8); delay_ms(50);
	dynamixelApi_connect(15); delay_ms(50);

	dynamixelApi_connect(5); delay_ms(50);
	dynamixelApi_connect(6); delay_ms(50);

	dynamixelApi_connect(9); delay_ms(50);
	dynamixelApi_connect(7); delay_ms(50);
	int i;
	for(i=0;i<dynamixelApi_countActuators();i++) {
		dynamixelApi_setWheelMode(i, false);
	}
}
void flowerCatch() {
	flowerInit();
	int time = 0;
	while(1) {
		int irVal = dynamixelApi_CM510_getADC(0);
		int pos, pos2;
		if(irVal>10) {
			pos = 300;
			pos2 = 512;
			dynamixelApi_setGoalPos(0, pos);
			dynamixelApi_setGoalPos(1, pos2);
			dynamixelApi_setGoalPos(2, pos);
			dynamixelApi_setGoalPos(3, pos2);
			dynamixelApi_setGoalPos(4, pos);
			dynamixelApi_setGoalPos(5, pos2);
		}
		else {
			pos = 450;
			pos2 = 850;
			dynamixelApi_setGoalPos(0, pos);
			dynamixelApi_setGoalPos(1, pos2);
			dynamixelApi_setGoalPos(2, pos);
			dynamixelApi_setGoalPos(3, pos2);
			dynamixelApi_setGoalPos(4, pos);
			dynamixelApi_setGoalPos(5, pos2);
		}
		ase_printf("%i\n",irVal);
		delay_ms(100);
		time++;
	}
}


void flowerDance() {
	flowerInit();
	int time=0;
	while(1) {
		int pos = 200*sin(time/10.0f)+512;
		int pos2 = 200*sin(-time/10.0f)+512+100;
		if(pos2<512) pos2=512;
		dynamixelApi_setGoalPos(0, pos);
		dynamixelApi_setGoalPos(1, pos2);
		dynamixelApi_setGoalPos(2, pos);
		dynamixelApi_setGoalPos(3, pos2);
		dynamixelApi_setGoalPos(4, pos);
		dynamixelApi_setGoalPos(5, pos2);
		delay_ms(250);
		int irVal = dynamixelApi_CM510_getADC(0);
		ase_printf("%i\n", irVal);
		time++;
	}
}


void testDynamixel() {


}


void testTimer() {
	while(1) {
		if(dynamixelApi_getMsTime()%250==0) {
			dynamixelApi_CM510_toggleLed(0);
			while(dynamixelApi_getMsTime()%250==0);
		}
	}
}

typedef struct {
	char typeID;
	char counter;
} TestMsg;

static void handleMessage(char* topic, Event_t* event) {
	Msg_t* msg = (Msg_t*) event->val_prt;
	ase_printf("CM-510 got message of type %i with length %i on channel %i\n",msg->message[0],msg->messageSize,msg->channel);
/*	int length = msg->messageSize;
	int type;
	if(length>0) type = msg->message[0];
	else return;
	if(type==LPF_ASE_SET_RC_STATE) {
	  	LuiManager_setDeviceRCList((signed char*)(&msg->message[1]), msg->messageSize-1, -100);
	}*/
}

void testSendMessage() {
	TestMsg msg;
	msg.typeID = 'm';
	msg.counter = 0;
	while(1) {
		if(dynamixelApi_getMsTime()%500==0) {
			dynamixelApi_sendMessage((char*)&msg, sizeof(msg), 0);
			msg.counter++;
			while(dynamixelApi_getMsTime()%250==0);
		}
	}
}
/*#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#include <ase/zigbee.h>

#define LED_BAT 0x01
#define LED_TxD 0x02
#define LED_RxD 0x04
#define LED_AUX 0x08
#define LED_MANAGE 0x10
#define LED_PROGRAM 0x20
#define LED_PLAY 0x40
void testZigBee() {
	int RcvData;

	DDRC  = 0x7F;
	PORTC = 0x7E;

	PORTD &= ~0x80;	//PORT_LINK_PLUGIN = 0;   // no pull up
	PORTD &= ~0x20;	//PORT_ENABLE_RXD_LINK_PC = 0;
	PORTD |= 0x40;	//PORT_ENABLE_RXD_LINK_ZIGBEE = 1;

	zgb_initialize( 0 ); // Not using device index
	sei();			// Interrupt Enable

	while(1) {
		zgb_tx_data(9);
		if(zgb_rx_check() == 1)  {
			RcvData = zgb_rx_data();
			if(RcvData & RC100_BTN_1)
				PORTC &= ~LED_MANAGE;
			else
				PORTC |= LED_MANAGE;

			if(RcvData & RC100_BTN_2)
				PORTC &= ~LED_PROGRAM;
			else
				PORTC |= LED_PROGRAM;

			if(RcvData & RC100_BTN_3)
				PORTC &= ~LED_PLAY;
			else
				PORTC |= LED_PLAY;
		}
		myDelay(100000);
	}
}*/

TestMsg msg;

void act() {
	if(dynamixelApi_getMsTime()<1000) {
		msg.typeID = 'm';
		msg.counter = 0;
	}
	else if(dynamixelApi_getMsTime()%500==0) {
			dynamixelApi_sendMessage((char*)&msg, sizeof(msg), 0);
			msg.counter++;
			while(dynamixelApi_getMsTime()%500==0);
	}
}

//TODO ZigBee, PC Communication
void controller_init() {
	dynamixelApi_CM510_init();
	EventManager_subscribe(MSG_RECV_EVENT, handleMessage);
	EventManager_subscribe(ACT_EVENT, act);
	//ase_printf("*************************\n" );
	//ase_printf("*Dynamixel-CM510 Started*\n" );
	//ase_printf("************************\n" );
	//testButtons();
	//testBuzz();
	//testLed();
	//testSerial();
	//testSerialReadLine();
	//testIR();
	//testMic();
	//testDynamixel();
	flowerCatch();
	//flowerDance();
	//testTimer();
	//testZigBee();
	//testSendMessage();
}
