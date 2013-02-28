#include <string.h>
#include <ase/targets/fable/comm/frame.h>
//#include "protocol/protocol.h"
#include <ase/targets/fable/comm/interfaces.h>
//#include "drivers/light.h"
pinterface interfaces[MAX_INTERFACES];
uint8_t active_interfaces;
frame connection_frame;

/*Send a byte on the channel. Escapes the byte if necessary*/
void IF_send_byte(uint8_t byte, pinterface interface) {
	if(byte == ESC || byte == START_FLAG || byte == END_FLAG)
		interface->put(ESC);

	interface->put(byte);
}

bool IF_send_frame(pframe frm, uint8_t channel) {
	pinterface interface;

	interface = IF_get_from_channel(channel);
	if (interface == NULL)
		return false;

	// Set crc;
	frame_set_crc(frm);
	// Start flag
//	printf("interface %d sending a frame\n",channel);
	interface->put(START_FLAG);
	// Send the frame
	for(uint16_t i=0; i<FRAME_HEADER+frm->info_size; i++) {
		IF_send_byte(frm->data[i], interface);
	}

	// Send checksum
	IF_send_byte(frm->crc&0xFF, interface);
	IF_send_byte(frm->crc>>8, interface);

	// End flag
	interface->put(END_FLAG);
//	printf("if channel %d sent frame\n",channel);
//	return interface->sync();
	return 1;
}

pinterface IF_get_from_channel(uint8_t channel) {
	if (channel < active_interfaces) {
		return interfaces[channel];
	}

	return NULL;
}

/*Returns whether a fully correct frame is received or not*/
frame* IF_receive_frame(uint8_t channel) {
	pinterface interface;

	interface = IF_get_from_channel(channel);
	if (interface == NULL) {
//		set_one_led(LIGHT_YELLOW, 5);
//		set_one_led(LIGHT_YELLOW, 7);
//		delay(2);
		return NULL;
	}
	while(interface->poll()) {	//As long as there are bytes to receive
		uint8_t tmp = interface->get();
		if((interface->esc_flag) || ((tmp != ESC)&& (tmp != START_FLAG) && (tmp != END_FLAG))) {
			interface->esc_flag = false;
			interface->buf[interface->buf_ptr] = tmp;

			if(interface->buf_ptr < CONNECTION_BUFFER_SIZE)//Check for overflow
				interface->buf_ptr++;
		}
		else {
			switch(tmp) {
				case ESC:			//Set ESC flag
					interface->esc_flag = true;
					break;
				case START_FLAG: 	//Restart
					interface->buf_ptr = 0;
					//confrm->info_size = 0;
					break;
				case END_FLAG: {
					if (interface->buf_ptr <= (MAX_INFO_SIZE+FRAME_HEADER))
						memcpy(&connection_frame.data, interface->buf, interface->buf_ptr);

					// copy last two bytes to crc
					connection_frame.channel = channel;
					connection_frame.crc = ((uint16_t*)(&interface->buf[interface->buf_ptr-2]))[0];
					connection_frame.info_size = interface->buf_ptr;
					// frame received, set correct info size without header
					if(connection_frame.info_size >= (FRAME_HEADER+2))
						connection_frame.info_size -= (FRAME_HEADER+2);

					if (connection_frame.info_size > MAX_INFO_SIZE){
						return NULL;
					}

					return &connection_frame;

				}
				default:
					break;
			}
		}
	}
	return NULL;
}

bool IF_is_local(uint8_t channel) {
	pinterface connection;

	connection = IF_get_from_channel(channel);

	if (connection != NULL)
		return connection->is_local;

	return false;
}

void IF_add(pinterface connection) {
	if(active_interfaces>=MAX_INTERFACES)
		return;
	interfaces[active_interfaces] = connection;
	interfaces[active_interfaces]->esc_flag = false;
	interfaces[active_interfaces]->buf_ptr = 0;
	active_interfaces++;
}

uint8_t IF_active_interfaces(void) {
	return active_interfaces;
}

void IF_init(void) {
	active_interfaces = 0;
}
