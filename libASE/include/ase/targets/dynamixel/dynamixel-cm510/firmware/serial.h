#ifndef _SERIAL_HEADER
#define _SERIAL_HEADER

#ifdef __cplusplus
extern "C" {
#endif


void serial_initialize(long ubrr);
void serial_write( unsigned char *pData, int numbyte );
unsigned char serial_read( unsigned char *pData, int numbyte );
int serial_get_qstate(void);
unsigned char serial_get_queue(void);
void serial_put_queue( unsigned char data );
#ifdef __cplusplus
}
#endif

#endif
