#ifndef __FRAME_H_
#define __FRAME_H_

#include <avr/io.h>
#include <stdint.h>

#define MAX_INFO_SIZE	25	// The maximum number of data bytes in a frame exclusive all headers

#define FRAME_HEADER	6	// Number of header bytes in the frame 
							// control (1byte) + frame_nr (1byte) + addr_to (2bytes) + addr_frm (2bytes) = 6bytes
typedef struct frame {
	uint16_t info_size;		// Number of info bytes in the packet
	uint8_t channel;		// The channel where this packet was received
	uint16_t crc;			// CRC of the frame
	uint8_t data[0];		// Used to indicate that the packet starts here
	// frame
	uint8_t control;		// The command of the frame
	uint8_t frame_nr;		// The number of this frame, used in the history
	uint16_t address_to;		// Address of the receiver
	uint16_t address_from;		// Address of the transmitter
	uint8_t info[MAX_INFO_SIZE]; // Info, data that is transmitted
} frame, *pframe;

#endif

void frame_set_crc(pframe frm);
uint16_t frame_calculate_crc(pframe frm);
