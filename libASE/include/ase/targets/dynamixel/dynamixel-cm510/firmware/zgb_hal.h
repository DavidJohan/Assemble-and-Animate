#ifndef _ZIGBEE_HAL_HEADER
#define _ZIGBEE_HAL_HEADER

#ifdef __cplusplus
extern "C" {
#endif

int zgb_hal_open( int devIndex, float baudrate );
void zgb_hal_close(void);
int zgb_hal_tx( unsigned char *pPacket, int numPacket );
int zgb_hal_rx( unsigned char *pPacket, int numPacket );

#ifdef __cplusplus
}
#endif

#endif
