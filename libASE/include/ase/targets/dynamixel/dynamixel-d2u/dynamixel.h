#ifndef _DYNAMIXEL_HEADER
#define _DYNAMIXEL_HEADER

/*
#ifdef DYNAMIXEL_EXPORTS
#define DXL_API __declspec(dllexport)
#else
#define DXL_API __declspec(dllimport)
#endif

#define CALL_TYPE	__stdcall
*/
#define DXL_API
#define CALL_TYPE

#ifdef __cplusplus
extern "C" {
#endif


///////////// device control methods ////////////////////////
DXL_API int CALL_TYPE dxl_initialize();
DXL_API void CALL_TYPE dxl_terminate();
DXL_API int CALL_TYPE dxl_get_baud();
DXL_API void CALL_TYPE dxl_set_baud( int baudnum );


///////////// set/get packet methods //////////////////////////
#define MAXNUM_TXPARAM		(150)
#define MAXNUM_RXPARAM		(60)
DXL_API extern unsigned char gbInstructionPacket[MAXNUM_TXPARAM+10];
DXL_API extern unsigned char gbStatusPacket[MAXNUM_RXPARAM+10];
#define ID					(2)
#define LENGTH				(3)
#define INSTRUCTION			(4)
#define ERRBIT				(4)
#define PARAMETER			(5)
#define DEFAULT_BAUDNUMBER	(1)

DXL_API void CALL_TYPE dxl_set_txpacket_id( int id );
#define BROADCAST_ID		(254)

DXL_API void CALL_TYPE dxl_set_txpacket_instruction( int instruction );
#define INST_PING			(1)
#define INST_READ			(2)
#define INST_WRITE			(3)
#define INST_REG_WRITE		(4)
#define INST_ACTION			(5)
#define INST_RESET			(6)
#define INST_SYNC_WRITE		(131)

DXL_API void CALL_TYPE dxl_set_txpacket_parameter( int index, int value );
DXL_API void CALL_TYPE dxl_set_txpacket_length( int length );

DXL_API int CALL_TYPE dxl_get_rxpacket_error( int errbit );
#define ERRBIT_VOLTAGE		(1)
#define ERRBIT_ANGLE		(2)
#define ERRBIT_OVERHEAT		(4)
#define ERRBIT_RANGE		(8)
#define ERRBIT_CHECKSUM		(16)
#define ERRBIT_OVERLOAD		(32)
#define ERRBIT_INSTRUCTION	(64)

DXL_API int CALL_TYPE dxl_get_rxpacket_length();
DXL_API int CALL_TYPE dxl_get_rxpacket_parameter( int index );


// utility for value
DXL_API int CALL_TYPE dxl_makeword( int lowbyte, int highbyte );
DXL_API int CALL_TYPE dxl_get_lowbyte( int word );
DXL_API int CALL_TYPE dxl_get_highbyte( int word );


////////// packet communication methods ///////////////////////
DXL_API void CALL_TYPE dxl_tx_packet();
DXL_API void CALL_TYPE dxl_rx_packet();
DXL_API void CALL_TYPE dxl_txrx_packet();

DXL_API int CALL_TYPE dxl_get_result();
#define	COMM_TXSUCCESS		(0)
#define COMM_RXSUCCESS		(1)
#define COMM_TXFAIL			(2)
#define COMM_RXFAIL			(3)
#define COMM_TXERROR		(4)
#define COMM_RXWAITING		(5)
#define COMM_RXTIMEOUT		(6)
#define COMM_RXCORRUPT		(7)


//////////// high communication methods ///////////////////////
DXL_API void CALL_TYPE dxl_ping( int id );
DXL_API int CALL_TYPE dxl_read_byte( int id, int address );
DXL_API void CALL_TYPE dxl_write_byte( int id, int address, int value );
DXL_API int CALL_TYPE dxl_read_word( int id, int address );
DXL_API void CALL_TYPE dxl_write_word( int id, int address, int value );


#ifdef __cplusplus
}
#endif

#endif
