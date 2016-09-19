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
const uint32_t  BufferSize[2] = {REVICEBUFFERSIZE,SENDBUFFERSIZE};
static uint8_t *pUartBuffer[2] = {UartReviceBuffer,UartSendBuffer};
static uint16_t UartBufferTop[2]    = { 0, 0 };
static uint16_t UartBufferBottom[2] = { 0, 0 };
static uint32_t UartBufferSize[2]   = { 0, 0 };
static uint8_t  UartBufferStatus[2] = { BUFFEREMPTY, BUFFEREMPTY};

/* Private functions ---------------------------------------------------------*/
static void    buffer_read_change_status( uint8_t revice_or_send_buffer) ;
static void    buffer_write_change_status( uint8_t revice_or_send_buffer) ;
static void    buffer_update_write_index( uint8_t revice_or_send_buffer, uint8_t Len );
static void    buffer_update_read_index( uint8_t revice_or_send_buffer, uint8_t Len );
static void    buffer_clear_element( uint8_t revice_or_send_buffer );
static uint8_t buffer_get_data_from_buffer(uint8_t revice_or_send_buffer, uint16_t index);
static void    buffer_store_data_to_buffer(uint8_t revice_or_send_buffer, uint16_t index,uint8_t data);

/******************************************************************************
  Function:buffer_get_buffer_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_buffer_status( uint8_t revice_or_send_buffer )
{
		return UartBufferStatus[revice_or_send_buffer];
}

/******************************************************************************
  Function:buffer_get_data_from_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_data_from_buffer( uint8_t revice_or_send_buffer, uint16_t index )
{
	if( index < BufferSize[revice_or_send_buffer] )
		return pUartBuffer[revice_or_send_buffer][index];
	else
		return 
			pUartBuffer[revice_or_send_buffer][index-BufferSize[revice_or_send_buffer]];
}

/******************************************************************************
  Function:buffer_store_data_to_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void buffer_store_data_to_buffer( uint8_t revice_or_send_buffer, uint16_t index, uint8_t data)
{
	if( index < BufferSize[revice_or_send_buffer] )
		pUartBuffer[revice_or_send_buffer][index] = data;
	else 
		pUartBuffer[revice_or_send_buffer][index-BufferSize[revice_or_send_buffer]] = data;
}


/******************************************************************************
  Function:buffer_read_change_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_read_change_status( uint8_t revice_or_send_buffer) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = buffer_get_buffer_status(revice_or_send_buffer);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			break;
		
		case BUFFERUSEING:
			{
				if(serial_ringbuffer_get_usage_rate(revice_or_send_buffer) <= USAGE_TATE_EREMPTY)
					UartBufferStatus[revice_or_send_buffer] = BUFFEREMPTY;
				else
					UartBufferStatus[revice_or_send_buffer] = BUFFERUSEING;
			}
			break;
			
		case BUFFERFULL:
			{
					UartBufferStatus[revice_or_send_buffer] = BUFFERUSEING;
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
static void buffer_write_change_status( uint8_t revice_or_send_buffer) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = buffer_get_buffer_status(revice_or_send_buffer);
	
	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			{
					UartBufferStatus[revice_or_send_buffer] = BUFFERUSEING;
			}
			break;
		
		case BUFFERUSEING:
			{
				if(serial_ringbuffer_get_usage_rate(revice_or_send_buffer) >= USAGE_TATE_FULL)
					UartBufferStatus[revice_or_send_buffer] = BUFFERFULL;
				else
					UartBufferStatus[revice_or_send_buffer] = BUFFERUSEING;
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
static void buffer_update_write_index( uint8_t revice_or_send_buffer, uint8_t Len )
{ 
    uint16_t tmp = 0;
	
	UartBufferSize[revice_or_send_buffer] += Len; 
	  
	tmp = UartBufferTop[revice_or_send_buffer] + Len;
	  
	if(tmp > BufferSize[revice_or_send_buffer])
		UartBufferTop[revice_or_send_buffer] = tmp - BufferSize[revice_or_send_buffer];
	else
		UartBufferTop[revice_or_send_buffer] = tmp;
}

/******************************************************************************
  Function:buffer_update_read_index
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_update_read_index( uint8_t revice_or_send_buffer, uint8_t Len )
{
  uint16_t tmp = 0;
	
	UartBufferSize[revice_or_send_buffer] -= Len;
	  
	tmp = UartBufferBottom[revice_or_send_buffer] + Len;
	  
	if(tmp > BufferSize[revice_or_send_buffer])
		UartBufferBottom[revice_or_send_buffer] = tmp - BufferSize[revice_or_send_buffer];
	else
		UartBufferBottom[revice_or_send_buffer] = tmp;
}

/******************************************************************************
  Function:buffer_clear_element
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_clear_element( uint8_t revice_or_send_buffer )
{
	uint8_t i;
	uint8_t MessageLen = buffer_get_data_from_buffer( revice_or_send_buffer,
												UartBufferBottom[revice_or_send_buffer]+6 ) + 9;
	
	for(i=0;i<MessageLen;i++)
	{
		buffer_store_data_to_buffer(revice_or_send_buffer,
				UartBufferBottom[revice_or_send_buffer]+i,0);
	}
}

/******************************************************************************
  Function:serial_ringbuffer_write_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_write_data(uint8_t revice_or_send_buffer, Uart_MessageTypeDef *data)
{
	uint8_t i;
	uint8_t *pdata = (uint8_t *)data;
	uint8_t MessageLen = *(pdata+6) + 6;
	
	for(i=0;i<=MessageLen;i++)
	{
		buffer_store_data_to_buffer(revice_or_send_buffer, 
			UartBufferTop[revice_or_send_buffer]+i,*pdata);
		pdata++;
	}
	
	buffer_store_data_to_buffer(revice_or_send_buffer, 
		UartBufferTop[revice_or_send_buffer]+i+0,data->XOR);
	
	buffer_store_data_to_buffer(revice_or_send_buffer, 
		UartBufferTop[revice_or_send_buffer]+i+1,data->END);
	
	buffer_update_write_index( revice_or_send_buffer, MessageLen+3);
	
	buffer_write_change_status(revice_or_send_buffer);
}

/******************************************************************************
  Function:serial_ringbuffer_write_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_write_data1(uint8_t revice_or_send_buffer, uint8_t *data)
{
	uint8_t i;
	uint8_t *pdata = (uint8_t *)data;
	uint8_t MessageLen = *(pdata+6) + 6;
	
	for(i=0;i<=MessageLen;i++)
	{
		buffer_store_data_to_buffer(revice_or_send_buffer, 
			UartBufferTop[revice_or_send_buffer]+i,*pdata);
		pdata++;
	}
	
	buffer_store_data_to_buffer(revice_or_send_buffer, 
			UartBufferTop[revice_or_send_buffer]+i+0,*pdata++);
	buffer_store_data_to_buffer(revice_or_send_buffer, 
			UartBufferTop[revice_or_send_buffer]+i+1,*pdata++);
	
	buffer_update_write_index( revice_or_send_buffer, MessageLen+3);
	
	buffer_write_change_status(revice_or_send_buffer);
}

/******************************************************************************
  Function:serial_ringbuffer_read_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_read_data( uint8_t revice_or_send_buffer, Uart_MessageTypeDef *data )
{
		uint8_t i;
	  uint8_t *pdata = (uint8_t *)data;
	
	  uint8_t MessageLen = buffer_get_data_from_buffer( revice_or_send_buffer,
														UartBufferBottom[revice_or_send_buffer]+6) + 6;
		
		for(i=0;i<=MessageLen;i++)
		{
			*pdata = buffer_get_data_from_buffer(revice_or_send_buffer, 
									UartBufferBottom[revice_or_send_buffer]+i);
			pdata++;
		}
		
		data->XOR = buffer_get_data_from_buffer(revice_or_send_buffer, 
										UartBufferBottom[revice_or_send_buffer]+i+0);
		
		data->END = buffer_get_data_from_buffer(revice_or_send_buffer, 
										UartBufferBottom[revice_or_send_buffer]+i+1);
		
		buffer_clear_element(revice_or_send_buffer);
		
		buffer_update_read_index(revice_or_send_buffer, MessageLen+3);
		
		buffer_read_change_status(revice_or_send_buffer);
}

/******************************************************************************
  Function:serial_ringbuffer_read_data1
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_read_data1( uint8_t revice_or_send_buffer, uint8_t *data )
{
		uint8_t i;
	  uint8_t *pdata = (uint8_t *)data;
	
	  uint8_t MessageLen = buffer_get_data_from_buffer( revice_or_send_buffer,
														UartBufferBottom[revice_or_send_buffer]+6) + 6;
		
		for(i=0;i<=MessageLen;i++)
		{
			*pdata = buffer_get_data_from_buffer(revice_or_send_buffer, 
			             UartBufferBottom[revice_or_send_buffer]+i);
			pdata++;
		}
		
		*pdata++ = buffer_get_data_from_buffer(revice_or_send_buffer, 
		               UartBufferBottom[revice_or_send_buffer]+i+0);
		
		*pdata++ = buffer_get_data_from_buffer(revice_or_send_buffer, 
		                UartBufferBottom[revice_or_send_buffer]+i+1);
		
		buffer_clear_element(revice_or_send_buffer);
		
		buffer_update_read_index(revice_or_send_buffer, MessageLen+3);
		
		buffer_read_change_status(revice_or_send_buffer);
}


/******************************************************************************
  Function:serial_ringbuffer_read_data1
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t serial_ringbuffer_get_usage_rate(uint8_t revice_or_send_buffer)
{
	uint8_t usage_rate = 0;
	
	usage_rate = UartBufferSize[revice_or_send_buffer]*100/BufferSize[revice_or_send_buffer];
	
	return usage_rate;
}

/**************************************END OF FILE****************************/
