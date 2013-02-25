#ifndef _EXT_EE_H
#define _EXT_EE_H

#define MAX_RETRIES 10
#define EE_ADDR 0xA0//using only block 0


uint8_t ee_write_byte(uint8_t addr_highbyte,uint8_t addr_lowbyte, uint8_t data);
uint8_t ee_read_byte(uint8_t addr_highbyte,uint8_t addr_lowbyte);

uint8_t ee_seq_read(uint8_t howmanybytes,uint8_t* thatmanybytes,uint8_t addr_highbyte,uint8_t addr_lowbyte);
uint8_t ee_seq_write(uint8_t howmanybytes,uint8_t* thatmanybytes,uint8_t addr_highbyte,uint8_t addr_lowbyte);


#endif
