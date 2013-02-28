#ifndef __CONNECTION_H_
#define __CONNECTION_H_

#include "frame.h"
#include "../firmware/stddefs.h"

#define MAX_INTERFACES		5
#define ALL_CONNECTIONS		0xFF
#if defined BOOTLOADER
	#define CONNECTION_BUFFER_SIZE	300
#else
	#define CONNECTION_BUFFER_SIZE 	100
#endif
//#define MAX_INFO_SIZE		50

// Protocol constants
#define ESC 		0x1B
#define START_FLAG 	0x7E
#define END_FLAG 	0x7F

/*typedef struct frame {
	uint16_t info_size;
	uint8_t data[0];
	// frame
	uint8_t control;
	uint8_t frame_nr;
	uint16_t addr_to;		//Address of the receiver
	uint16_t addr_frm;		//The address where this package was send from
	uint8_t info[MAX_INFO_SIZE];
	uint16_t crc;			//Make sure that there are room enough for crc
} frame, *pframe;*/

/*typedef struct frame {
	uint16_t info_size;
	// frame
	uint8_t control;
	uint8_t frame_nr;
	uint16_t addr_to;		//Address of the receiver
	uint16_t addr_frm;		//The address where this package was send from
	uint8_t info[MAX_INFO_SIZE];
	uint16_t crc;
} frame, *pframe;*/

typedef struct {
	bool is_local;
	void(*put)(uint8_t);
	uint8_t(*poll)(void);
	uint8_t(*get)(void);
	bool(*sync)(void);
	bool esc_flag;
	uint16_t buf_ptr;
	uint8_t buf[CONNECTION_BUFFER_SIZE];
} interface, *pinterface;

bool IF_send_frame(pframe frm, uint8_t channel);
frame* IF_receive_frame(uint8_t chn);
pinterface IF_get_from_channel(uint8_t channel);
bool IF_is_local(uint8_t channel);
void IF_add(pinterface connection);
uint8_t IF_active_interfaces(void);
void IF_init(void);

#endif
