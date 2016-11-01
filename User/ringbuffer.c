/**
  ******************************************************************************
  * @file   	ringbuffer.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	ringbuffer
  ******************************************************************************
  */
#include "ringbuffer.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t  UartReviceBuffer[REVICEBUFFERSIZE];
static uint8_t  UartSendBuffer[SENDBUFFERSIZE];
static uint8_t  SpireviceBuffer[SPIBUFFERSIZE];
const uint32_t  BufferSize[RINGBUFFERSUM]       = {REVICEBUFFERSIZE,SENDBUFFERSIZE,SPIBUFFERSIZE};
static uint8_t *pUartBuffer[RINGBUFFERSUM]      = {UartReviceBuffer,UartSendBuffer,SpireviceBuffer};
static uint16_t UartBufferTop[RINGBUFFERSUM]    = { 0, 0, 0 };
static uint16_t UartBufferBottom[RINGBUFFERSUM] = { 0, 0, 0 };
static uint32_t UartBufferSize[RINGBUFFERSUM]   = { 0, 0, 0 };
static uint8_t  UartBufferStatus[RINGBUFFERSUM] = { BUFFEREMPTY, BUFFEREMPTY, BUFFEREMPTY};

/* Private functions ---------------------------------------------------------*/
static void    buffer_read_change_status( uint8_t sel_buffer) ;
static void    buffer_write_change_status( uint8_t sel_buffer) ;
static void    buffer_update_write_index( uint8_t sel_buffer, uint8_t Len );
static void    buffer_update_read_index( uint8_t sel_buffer, uint8_t Len );
static uint8_t buffer_get_data_from_buffer(uint8_t sel_buffer, uint16_t index);
static void    buffer_store_data_to_buffer(uint8_t sel_buffer, uint16_t index,uint8_t data);

/******************************************************************************
  Function:buffer_get_buffer_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_buffer_status( uint8_t sel_buffer )
{
		return UartBufferStatus[sel_buffer];
}

/******************************************************************************
  Function:buffer_get_data_from_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_data_from_buffer( uint8_t sel_buffer, uint16_t index )
{
	uint8_t data;

	if( index < BufferSize[sel_buffer] )
	{
		data = pUartBuffer[sel_buffer][index];
		pUartBuffer[sel_buffer][index] = 0;
	}
	else
	{
		data = pUartBuffer[sel_buffer][index-BufferSize[sel_buffer]];
		pUartBuffer[sel_buffer][index-BufferSize[sel_buffer]] = 0;
	}
	return data;
}

/******************************************************************************
  Function:buffer_store_data_to_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void buffer_store_data_to_buffer( uint8_t sel_buffer, uint16_t index, uint8_t data)
{
	if( index < BufferSize[sel_buffer] )
		pUartBuffer[sel_buffer][index] = data;
	else
		pUartBuffer[sel_buffer][index-BufferSize[sel_buffer]] = data;
}


/******************************************************************************
  Function:buffer_read_change_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_read_change_status( uint8_t sel_buffer)
{
	uint8_t bufferstatus = 0;

	bufferstatus = buffer_get_buffer_status(sel_buffer);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			break;

		case BUFFERUSEING:
			{
				if((serial_ringbuffer_get_usage_rate(sel_buffer) <= USAGE_TATE_EREMPTY) &&
					 (UartBufferTop[sel_buffer] == UartBufferBottom[sel_buffer]))
					UartBufferStatus[sel_buffer] = BUFFEREMPTY;
				else
					UartBufferStatus[sel_buffer] = BUFFERUSEING;
			}
			break;

		case BUFFERFULL:
			{
					UartBufferStatus[sel_buffer] = BUFFERUSEING;
			}
			break;

		default:
			break;
	}
}

/******************************************************************************
  Function:buffer_write_change_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_write_change_status( uint8_t sel_buffer)
{
	uint8_t bufferstatus = 0;

	bufferstatus = buffer_get_buffer_status(sel_buffer);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			{
					UartBufferStatus[sel_buffer] = BUFFERUSEING;
			}
			break;

		case BUFFERUSEING:
			{
				if(serial_ringbuffer_get_usage_rate(sel_buffer) >= USAGE_TATE_FULL)
					UartBufferStatus[sel_buffer] = BUFFERFULL;
				else
					UartBufferStatus[sel_buffer] = BUFFERUSEING;
			}
			break;

		case BUFFERFULL:
			break;

		default:
			break;
	}
}
/******************************************************************************
  Function:buffer_update_write_index
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_update_write_index( uint8_t sel_buffer, uint8_t Len )
{
    uint16_t tmp = 0;

	UartBufferSize[sel_buffer] += Len;

	tmp = UartBufferTop[sel_buffer] + Len;

	if(tmp > BufferSize[sel_buffer])
		UartBufferTop[sel_buffer] = tmp - BufferSize[sel_buffer];
	else
		UartBufferTop[sel_buffer] = tmp;
}

/******************************************************************************
  Function:buffer_update_read_index
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_update_read_index( uint8_t sel_buffer, uint8_t Len )
{
  uint16_t tmp = 0;

	UartBufferSize[sel_buffer] -= Len;

	tmp = UartBufferBottom[sel_buffer] + Len;

	if(tmp > BufferSize[sel_buffer])
		UartBufferBottom[sel_buffer] = tmp - BufferSize[sel_buffer];
	else
		UartBufferBottom[sel_buffer] = tmp;
}

/******************************************************************************
  Function:serial_ringbuffer_write_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_write_data(uint8_t sel_buffer, Uart_MessageTypeDef *data)
{
	uint8_t i;
	uint8_t *pdata = (uint8_t *)data;
	uint8_t MessageLen = *(pdata+6) + 6;

	for(i=0;i<=MessageLen;i++)
	{
		buffer_store_data_to_buffer(sel_buffer,
			UartBufferTop[sel_buffer]+i,*pdata);
		pdata++;
	}

	buffer_store_data_to_buffer(sel_buffer,
		UartBufferTop[sel_buffer]+i+0,data->XOR);

	buffer_store_data_to_buffer(sel_buffer,
		UartBufferTop[sel_buffer]+i+1,data->END);

	buffer_update_write_index( sel_buffer, MessageLen+3);

	buffer_write_change_status(sel_buffer);
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		App RF 消息缓存处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_data_to_buffer( uint8_t sel_buffer, uint8_t SpiMessage[], uint8_t send_data_status )
{
	uint8_t Len, *pdata, i;

	Len = SpiMessage[14];
	pdata = SpiMessage;

	for(i=0;i<Len+17;i++)
	{
		buffer_store_data_to_buffer(sel_buffer,
				UartBufferTop[sel_buffer]+i,*pdata);
			pdata++;
	}

	buffer_store_data_to_buffer(sel_buffer,
				UartBufferTop[sel_buffer]+i++,send_data_status);

	buffer_update_write_index( sel_buffer, Len+17+1);

	buffer_write_change_status(sel_buffer);
}

void spi_read_data_from_buffer( uint8_t sel_buffer, uint8_t SpiMessage[] )
{
	uint8_t *pdata, i;
	uint8_t MessageLen = buffer_get_data_from_buffer( sel_buffer,
														UartBufferBottom[sel_buffer]+14) + 17;
	pdata = SpiMessage;

	for(i=0;i<=MessageLen;i++)
	{
		*pdata = buffer_get_data_from_buffer(sel_buffer,
								UartBufferBottom[sel_buffer]+i);
		buffer_store_data_to_buffer(sel_buffer,
				UartBufferBottom[sel_buffer]+i,0);
		pdata++;
	}

	buffer_update_read_index(sel_buffer, MessageLen+1);

	buffer_read_change_status(sel_buffer);
}

/******************************************************************************
  Function:serial_ringbuffer_read_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_read_data( uint8_t sel_buffer, Uart_MessageTypeDef *data )
{
		uint8_t i;
	  uint8_t *pdata = (uint8_t *)data;

	  uint8_t MessageLen = buffer_get_data_from_buffer( sel_buffer,
														UartBufferBottom[sel_buffer]+6) + 6;

		for(i=0;i<=MessageLen;i++)
		{
			*pdata = buffer_get_data_from_buffer(sel_buffer,
									UartBufferBottom[sel_buffer]+i);
			pdata++;
		}
		data->LEN = MessageLen - 6;

		data->XOR = buffer_get_data_from_buffer(sel_buffer,
										UartBufferBottom[sel_buffer]+i+0);

		data->END = buffer_get_data_from_buffer(sel_buffer,
										UartBufferBottom[sel_buffer]+i+1);

		buffer_update_read_index(sel_buffer, MessageLen+3);

		buffer_read_change_status(sel_buffer);
}

/******************************************************************************
  Function:serial_ringbuffer_get_usage_rate
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t serial_ringbuffer_get_usage_rate(uint8_t sel_buffer)
{
	uint8_t usage_rate = 0;

	usage_rate = UartBufferSize[sel_buffer]*100/BufferSize[sel_buffer];

	return usage_rate;
}

/**************************************END OF FILE****************************/
