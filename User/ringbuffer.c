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
static uint8_t  UartReviceBuffer[BUFFERSIZE];
static uint16_t UartReceiveBufferTop = 0;
static uint16_t UartReceiveBufferTottom = 0;
static uint32_t UartReceiveReadIndex = 0;
static uint32_t UartReceiveWriteIndex = 0;
static uint8_t  UartReceiveBufferStatus = BUFFEREMPTY;

static uint8_t  UartSendBuffer[BUFFERSIZE];
static uint16_t UartSendBufferTop = 0;
static uint16_t UartSendBufferTottom = 0;
static uint32_t UartSendReadIndex = 0;
static uint32_t UartSendWriteIndex = 0;
static uint8_t  UartSendBufferStatus = BUFFEREMPTY;

/* Private functions ---------------------------------------------------------*/
static void    buffer_read_change_status(void) ;
static void    buffer_write_change_status(void) ;
static void    buffer_update_write_index( uint8_t Len );
static void    buffer_update_read_index( uint8_t Len );
static void    buffer_clear_element(void);
static uint8_t buffer_get_data_from_buffer(uint16_t index);
static void    buffer_store_data_to_buffer(uint16_t index,uint8_t data);

/******************************************************************************
  Function:buffer_get_buffer_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_buffer_status(void)
{
	return UartReceiveBufferStatus;
}

/******************************************************************************
  Function:buffer_get_read_index
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint32_t buffer_get_read_index(void)
{
	return UartReceiveReadIndex;
}

/******************************************************************************
  Function:GetBufferUartReceiveWriteIndex
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint32_t buffer_get_write_index(void)
{
	return UartReceiveWriteIndex;
}

/******************************************************************************
  Function:buffer_get_data_from_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_data_from_buffer(uint16_t index)
{
	if( index < BUFFERSIZE )
		return UartReviceBuffer[index];
	else
		return 
			UartReviceBuffer[index-BUFFERSIZE];
}

/******************************************************************************
  Function:buffer_store_data_to_buffer
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void buffer_store_data_to_buffer(uint16_t index,uint8_t data)
{
	if( index < BUFFERSIZE )
		UartReviceBuffer[index] = data;
	else 
		UartReviceBuffer[index-BUFFERSIZE] = data;
}


/******************************************************************************
  Function:buffer_read_change_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_read_change_status(void) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = buffer_get_buffer_status();

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			break;
		
		case BUFFERUSEING:
			{
				if(UartReceiveReadIndex == UartReceiveWriteIndex)
					UartReceiveBufferStatus = BUFFEREMPTY;
				else
					UartReceiveBufferStatus = BUFFERUSEING;
			}
			break;
			
		case BUFFERFULL:
			{
					UartReceiveBufferStatus = BUFFERUSEING;
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
static void buffer_write_change_status(void) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = buffer_get_buffer_status();
	
	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			{
					UartReceiveBufferStatus = BUFFERUSEING;
			}
			break;
		
		case BUFFERUSEING:
			{
				if(UartReceiveWriteIndex == UartReceiveReadIndex)
					UartReceiveBufferStatus = BUFFERFULL;
				else
					UartReceiveBufferStatus = BUFFERUSEING;
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
static void buffer_update_write_index( uint8_t Len )
{ 
    uint16_t tmp = 0;
	
	  tmp = UartReceiveBufferTop + Len;
	  
	if(tmp > BUFFERSIZE)
		UartReceiveBufferTop = tmp - BUFFERSIZE;
	else
		UartReceiveBufferTop = tmp;

	UartReceiveWriteIndex++;
}

/******************************************************************************
  Function:buffer_update_read_index
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_update_read_index( uint8_t Len )
{
    uint16_t tmp = 0;
	
	  tmp = UartReceiveBufferTottom + Len;
	  
	  if(tmp > BUFFERSIZE)
		  UartReceiveBufferTottom = tmp - BUFFERSIZE;
	  else
		  UartReceiveBufferTottom = tmp;
	
	  UartReceiveReadIndex++;
}

/******************************************************************************
  Function:buffer_clear_element
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void buffer_clear_element(void)
{
	uint8_t i;
	uint8_t MessageLen = buffer_get_data_from_buffer(UartReceiveBufferTottom+6) + 9;
	
	for(i=0;i<MessageLen;i++)
	{
		buffer_store_data_to_buffer(UartReceiveBufferTottom+i,0);
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
void serial_ringbuffer_write_data(Uart_MessageTypeDef *data)
{
	uint8_t i;
	uint8_t *pdata = (uint8_t *)data;
	uint8_t MessageLen = *(pdata+6) + 6;
	
	for(i=0;i<MessageLen;i++)
	{
		buffer_store_data_to_buffer(UartReceiveBufferTop+i,*pdata);
		pdata++;
	}
	
	buffer_store_data_to_buffer(UartReceiveBufferTop+i+1,data->XOR);
	buffer_store_data_to_buffer(UartReceiveBufferTop+i+2,data->END);
	
	buffer_update_write_index(MessageLen+3);
	
	buffer_write_change_status();
}

/******************************************************************************
  Function:serial_ringbuffer_read_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_read_data(Uart_MessageTypeDef *data)
{
		uint8_t i;
	  uint8_t *pdata = (uint8_t *)data;
	
	  uint8_t MessageLen = buffer_get_data_from_buffer(UartReceiveBufferTottom+6) + 7;
		
		for(i=0;i<MessageLen;i++)
		{
			*pdata = buffer_get_data_from_buffer(UartReceiveBufferTottom+i);
			pdata++;
		}
		
		data->XOR = buffer_get_data_from_buffer(UartReceiveBufferTottom+i+1);
		data->END = buffer_get_data_from_buffer(UartReceiveBufferTottom+i+2);
		
		buffer_clear_element();
		
		buffer_update_read_index(MessageLen+2);
		
		buffer_read_change_status();
}
