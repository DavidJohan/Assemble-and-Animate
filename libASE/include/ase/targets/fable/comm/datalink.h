#ifndef __DATALINK_H_
#define __DATALINK_H_

//#include "protocol/protocol.h"

#include "crc16.h"
#include "frame.h"
//#include "utils/buffer.h"
#include "../firmware/stddefs.h"
#include "../firmware/timer.h"

#include <stdint.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/pgmspace.h>

//Datalink specific constants
#define MAX_CHANNELS	6	// Number of communications channels: 4xSerial+XBEE = 5

#define NUMBER_OF_RETRANSMISSIONS 	3
#define RETRANSMISSION_INTERVAL		500



enum channel_status {
	NO_CHANNEL,
	NO_NEIGHBOUR,
//	BOOTING,
//	BOOT_DONE,
//	BOOTED_HERE,
	CONNECTED,
};

typedef struct {
	uint8_t status;
	timer ack_timer;
	uint8_t retransmissions;
//	bool local_channel;
//	bool esc_flag;
	bool ack_received;
//	uint16_t boot_offset;	//Both used for temporary boot offset and calculating crc
//	frame *frame_in; 		//Pointer to the ingoing frame
//	service_ops *ops;	//Pointers to operations for this channel
//	uint8_t(*poll)(void);
//	uint8_t(*read)(void);
//	void(*write)(uint8_t);
} comm_channel;

//static volatile comm_channel channels[MAX_CHANNELS];

#define ERROR_NUMBERS 1
/*enum DLL_ERROR {
	ERROR
};*/

//Prototypes

uint8_t get_n_neighbours();

void DLL_receiver(void);
void DLL_transmitter(void);

bool DLL_send_frame(frame *frm, uint8_t channel);
void DLL_transmit_frame(frame *frm, uint8_t channel);
void DLL_change_channel_list(uint8_t first_channel, uint8_t channels_to_move);

void init_channels_buffers(void);
void DLL_init(void (*handler)(frame*));

void transmit_frame(uint8_t* info,uint8_t info_size,uint8_t control,uint8_t channel );

#endif
