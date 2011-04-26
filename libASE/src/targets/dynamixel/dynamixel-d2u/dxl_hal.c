#include <windows.h>
#include "ftd2xx.h"
#include <ase/dxl_hal.h>


#define LATENCY_TIME		1 //ms
#define IN_TRASFER_SIZE		512 //unsigned char

FT_HANDLE ghFt_Handle = 0;
float gfByteTransTime = 0.0f;
float gfRcvWaitTime = 0.0f;
LARGE_INTEGER gStartTime;


int dxl_hal_open()
{
	FT_STATUS ft_status;

	dxl_hal_close();

	ft_status = FT_Open( 1, &ghFt_Handle );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_ResetDevice( ghFt_Handle );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_SetDataCharacteristics( ghFt_Handle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_SetFlowControl( ghFt_Handle, FT_FLOW_NONE, (UCHAR)0, (UCHAR)0 );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_SetLatencyTimer( ghFt_Handle, LATENCY_TIME );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_SetUSBParameters( ghFt_Handle, IN_TRASFER_SIZE, 0 );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_SetTimeouts( ghFt_Handle, 0, 0 );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	ft_status = FT_Purge( ghFt_Handle, FT_PURGE_RX|FT_PURGE_TX );
	if( ft_status != FT_OK )
		goto DXL_HAL_OPEN_ERROR;

	return 1;

DXL_HAL_OPEN_ERROR:
	dxl_hal_close();
	return 0;
}

void dxl_hal_close()
{
	if( ghFt_Handle != 0 )
	{
		FT_Close( ghFt_Handle );
		ghFt_Handle = 0;
	}
}

int dxl_hal_set_baud( float baudrate )
{
	FT_STATUS ft_status;
	int iBaudrate;

	iBaudrate = (int)baudrate;
	ft_status = FT_SetBaudRate( ghFt_Handle, (DWORD)iBaudrate );
	if( ft_status != FT_OK )
		return 0;

	gfByteTransTime = (float)((1000.0f / baudrate) * 12.0f);
	return 1;
}

void dxl_hal_clear(void)
{
	FT_Purge( ghFt_Handle, FT_PURGE_RX );
}

int dxl_hal_tx( unsigned char *pPacket, int numPacket )
{
	FT_STATUS ft_status;
	DWORD dwNumToWrite = (DWORD)numPacket;
	DWORD dwNumWritten;

	ft_status = FT_Write( ghFt_Handle, (LPVOID)pPacket, dwNumToWrite, &dwNumWritten );
	if( ft_status == FT_IO_ERROR )
		return 0;

	return (int)dwNumWritten;
}

int dxl_hal_rx( unsigned char *pPacket, int numPacket )
{
	FT_STATUS ft_status;
	DWORD dwNumToRead;
	DWORD dwNumRead = 0;

	ft_status = FT_GetQueueStatus( ghFt_Handle, &dwNumToRead );
	if( ft_status != FT_OK )
		return 0;

	if( dwNumToRead > 0 )
	{
		ft_status = FT_Read( ghFt_Handle, (LPVOID)pPacket, dwNumToRead, &dwNumRead );
		if( ft_status == FT_IO_ERROR )
			return 0;
	}

	return (int)dwNumRead;
}

void dxl_hal_set_timeout( int NumRcvByte )
{
	QueryPerformanceCounter( &gStartTime );
	gfRcvWaitTime = (float)(gfByteTransTime*(float)NumRcvByte + 2*LATENCY_TIME + 2.0f);
}

int dxl_hal_timeout(void)
{
	LARGE_INTEGER end, freq;
	double time;

	QueryPerformanceCounter( &end );
	QueryPerformanceFrequency( &freq );

	time = (double)(end.QuadPart - gStartTime.QuadPart) / (double)freq.QuadPart;
	time *= 1000.0;

	if( time > gfRcvWaitTime )
		return 1;
	else if( time < 0 )
		QueryPerformanceCounter( &gStartTime );

	return 0;
}
