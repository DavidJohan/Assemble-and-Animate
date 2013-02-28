/*
 * frame.c
 *
 *  Created on: Mar 25, 2010
 *      Author: sandmann
 */

#include <ase/targets/fable/comm/frame.h>
#include <ase/targets/fable/comm/crc16.h>
uint16_t frame_calculate_crc(frame *frm) {
	uint16_t crc=0;
	for(uint16_t i=0; i < frm->info_size+FRAME_HEADER; i++) {
		crc = crc16_update(crc,frm->data[i]);
	}
	return crc;
}

void frame_set_crc(frame *frm) {
	frm->crc = frame_calculate_crc(frm);
}
