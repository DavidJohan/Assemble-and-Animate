//#include "protocol/protocol.h"
#include <stdlib.h>
#include <avr/io.h>
#include <stdio.h>

#include <ase/targets/fable/comm/interfaces.h>
#include <ase/targets/fable/comm/datalink.h>
#include <ase/targets/fable/comm/frame.h>
#include <ase/targets/fable/firmware/stddefs.h>
#include <ase/targets/fable/firmware/timer.h>


static comm_channel channels[MAX_CHANNELS];

#define RELIABLE_MASK 0x80

#define DLL_ACK 1
#define DLL_NACK 2


#define FRAME_BUF_SIZE	20
frame frame_buffer[FRAME_BUF_SIZE];

static volatile uint8_t frame_buffer_first = 0, frame_buffer_last = 0;
bool waiting_for_ack = false;
timer ack_timer;
uint8_t retransmissions=0;

static void (*upper_level_handler)(frame* f);

/*
 * Div transmitter functions
 */

uint8_t get_n_neighbours(){
	int nchannels=0;
	for(int i=0;i<MAX_CHANNELS;i++)
		if(channels[i].status==CONNECTED)
			nchannels+=1;
	return nchannels;
}

void DLL_deliver_frame(frame *frm, uint8_t channel) {
	// try to send the frame max 3 times
	uint8_t send_timeout = 3;
	do {
		if(IF_send_frame(frm, channel))
			return;
		delay_ms((rand() % 5)+2);
		send_timeout--;
	}
	while(send_timeout > 0);
}

//extern bool has_xbee;
//extern bool is_alive_activated;
void DLL_transmit_frame(frame *frm, uint8_t channel) {
	if((frm->control & RELIABLE_MASK) && IF_is_local(channel)) { // Not reliable on XBee
		//If reliable, put in buffer
		frame_buffer[frame_buffer_last].channel = channel;
		frame_buffer[frame_buffer_last].info_size = frm->info_size;
		frame_buffer[frame_buffer_last].control = frm->control | RELIABLE_MASK;
		frame_buffer[frame_buffer_last].frame_nr = frm->frame_nr;
		frame_buffer[frame_buffer_last].address_to = frm->address_to;
		frame_buffer[frame_buffer_last].address_from = frm->address_from;
		//Copy frame to frame buffer
		for(uint8_t i=0; i<frm->info_size && i<MAX_INFO_SIZE; i++) {
			frame_buffer[frame_buffer_last].info[i] = frm->info[i];
		}
		frame_buffer_last++;
		if(frame_buffer_last>=FRAME_BUF_SIZE)
			frame_buffer_last=0;
	} else {
		frm->control = (frm->control & ~RELIABLE_MASK);
		DLL_deliver_frame(frm, channel);
	}
	//	set_all_leds(LIGHT_RED);
}

/*
 * Div receiver functions
 */

bool sending = false;
void DLL_receiver(void) {
	pframe frm;
	for(uint8_t i=0; i<IF_active_interfaces(); i++) {
		frm = IF_receive_frame(i);
		if (frm != NULL) {
			if(frm->crc == frame_calculate_crc(frm)) {
				//				printf("received a valid frame on channel %d: ",frm->channel);
				channels[i].status=CONNECTED;
				printf("got neighbour on channel %d\n",i);
				switch(frm->control & ~RELIABLE_MASK) {
				case DLL_ACK:
					//Ack received
					waiting_for_ack = false;
					sending = false;
					frame_buffer_first++;
					if(frame_buffer_first >= FRAME_BUF_SIZE)
						frame_buffer_first=0;
					//set_one_led(LIGHT_GREEN, i*2);
					//set_one_led(0,15,0, i*2);
					//					printf("it is an ACK, framebufferfistr= %d, framebufferlast=%d \n",frame_buffer_first,frame_buffer_last);
					break;
				case DLL_NACK:
					waiting_for_ack = false;	//The transmitter will send the same frame again
					sending = false;
					//set_one_led(LIGHT_RED, i*2);
					//set_one_led(15,0,0, i*2);
					break;
				default:
					if((frm->control & RELIABLE_MASK) && IF_is_local(i)) {
						frame feedback_frm;

						feedback_frm.info_size = 0;
						feedback_frm.control = DLL_ACK;
						feedback_frm.frame_nr = frm->frame_nr;
						//						printf("sending ACK\n");
						IF_send_frame(&feedback_frm, i);
						//set_one_led(LIGHT_PURPLE, i*2);
						//set_one_led(15,0,15, i*2);

					}
					//set_one_led(LIGHT_OFF,4);

					// Process the frame
					//						NWL_process_frame(frm);
					if(upper_level_handler==NULL){
						printf("ERROR: DLL not initialized!\n");
						return;
					}
					upper_level_handler(frm);
					//					printf("saying %s\n",(char*)frm->info);
					break;
				}
			}
		}	// else { Ignore checksum errors }
	}
}

// Reliable packet transmitter
void DLL_transmitter(void) {
	//	if(frame_buffer_first != frame_buffer_last) {// Reliable packets are waiting
	if(frame_buffer_first < frame_buffer_last) {// Reliable packets are waiting
		//		printf("found packets waiting framebufferfistr= %d, framebufferlast=%d \n",frame_buffer_first,frame_buffer_last);
		if(!waiting_for_ack) {
			sending = true;
			waiting_for_ack = true;
			//			printf("trying to deliver frame on channel %d\n",frame_buffer[frame_buffer_first].channel);
			DLL_deliver_frame(&frame_buffer[frame_buffer_first], frame_buffer[frame_buffer_first].channel);
			//Clear timer and number of retransmits
			retransmissions = NUMBER_OF_RETRANSMISSIONS;
			timer_reset(&ack_timer);
			//set_one_led(LIGHT_GREEN, frame_buffer[frm_first].channel*2+1);
			//set_one_led(0,15,0, frame_buffer[frame_buffer_first].channel*2+1);
			//			printf("delivered frame,waiting for ack\n");
		}
		else if(timer_compare(&ack_timer, RETRANSMISSION_INTERVAL+(rand()%10))) {

			if((--retransmissions) == 0) {	//The frame couldn�t be delivered
				//				printf("retransmissions over, discardingframe on channel %d\n",frame_buffer[frame_buffer_first].channel);
				channels[frame_buffer[frame_buffer_first].channel].status=NO_NEIGHBOUR;
				printf("no neighbours on channel %d\n",frame_buffer[frame_buffer_first].channel);
				frame_buffer_first++;
				if(frame_buffer_first >= FRAME_BUF_SIZE)
					frame_buffer_first=0;
				waiting_for_ack = false;
				sending = false;
				//set_one_led(LIGHT_RED, frame_buffer[frm_first].channel*2+1);
				//set_one_led(15,0,0, frame_buffer[frame_buffer_first].channel*2+1);

			}
			else {
				//				printf("timeout, retrying to deliver frame on channel %d\n",frame_buffer[frame_buffer_first].channel);
				DLL_deliver_frame(&frame_buffer[frame_buffer_first], frame_buffer[frame_buffer_first].channel);
				//set_one_led(LIGHT_YELLOW, frame_buffer[frm_first].channel*2+1);
				//set_one_led(15,15,0, frame_buffer[frame_buffer_first].channel*2+1);

			}
			timer_reset(&ack_timer);
		}
	}
	else if(sending && timer_compare(&ack_timer, RETRANSMISSION_INTERVAL+(rand()%10))) {
		if((--retransmissions) == 0) {	//The frame couldn�t be delivered
			//			printf("retransmissions over, discardingframe on channel %d\n",frame_buffer[frame_buffer_first].channel);
			channels[frame_buffer[frame_buffer_first].channel].status=NO_NEIGHBOUR;
			printf("no neighbours on channel %d\n",frame_buffer[frame_buffer_first].channel);
			frame_buffer_first++;
			if(frame_buffer_first >= FRAME_BUF_SIZE)
				frame_buffer_first=0;
			waiting_for_ack = false;
			sending = false;
			//set_one_led(LIGHT_RED, frame_buffer[frm_first].channel*2+1);
			//set_one_led(15,0,0, frame_buffer[frame_buffer_first].channel*2+1);
		}
		else {
			//			printf("timeout, retrying to deliver frame on channel %d\n",frame_buffer[frame_buffer_first].channel);
			DLL_deliver_frame(&frame_buffer[frame_buffer_first], frame_buffer[frame_buffer_first].channel);
			//set_one_led(LIGHT_YELLOW, frame_buffer[frm_first].channel*2+1);
			//set_one_led(15,15,0, frame_buffer[frame_buffer_first].channel*2+1);

		}
		timer_reset(&ack_timer);
	}
}

/*
 * Initializing functions
 */
void DLL_init(void (*handler)(frame*)) {
	if(handler==NULL){
		printf("ERROR, cannot init DLL!\n");
		return;
	}
	upper_level_handler=handler;
	timer_reset(&ack_timer);
	//Initialize channels
	for(uint8_t i=0; i<MAX_CHANNELS; i++) {
		channels[i].retransmissions	= 0;
		channels[i].ack_received 	= true;
		channels[i].status=NO_NEIGHBOUR;
		timer_reset(&channels[i].ack_timer);
	}
}

void transmit_frame(uint8_t* info,uint8_t info_size,uint8_t control,uint8_t channel ){
	static int framenumber=0;
	frame frm;
	//Prepare frame
		frm.info_size = info_size;
		frm.control = control;        //Set the network layer control flag
//		if(reliable)
		if(1)
			frm.control = (frm.control|0x80);
		frm.frame_nr =   framenumber;      //Set frame nr
//		frm.address_to  = address_to;        //Insert the address of the receiver
		frm.address_to  = 2;        //Insert the address of the receiver
		frm.address_from = 1;      //Insert the address of this device

		for(uint8_t i=0; i<info_size; i++)  //Copy info
			frm.info[i] = ((uint8_t*)info)[i];

		DLL_transmit_frame(&frm,channel);
//		printf("transmitting frame\n");
}
