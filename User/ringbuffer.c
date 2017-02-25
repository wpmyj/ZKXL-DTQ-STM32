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
#include "string.h"

/* Private variables ---------------------------------------------------------*/
static uint8_t  UartReviceBuffer[REVICEBUFFERSIZE];
static uint8_t  UartSendBuffer[SENDBUFFERSIZE];
static uint8_t  SpireviceBuffer[SPIBUFFERSIZE];
static uint8_t  SpiIrqBuffer[SPIIRQBUFFERSIZE];
static uint8_t  PrintBuffer[PRINTBUFFERSIZE];
const uint32_t  BufferSize[RINGBUFFERSUM]      = {REVICEBUFFERSIZE,SENDBUFFERSIZE,SPIBUFFERSIZE,PRINTBUFFERSIZE,SPIIRQBUFFERSIZE};
static uint8_t *pBuffer[RINGBUFFERSUM]         = {UartReviceBuffer,UartSendBuffer,SpireviceBuffer,PrintBuffer,SpiIrqBuffer};
static volatile uint16_t Top[RINGBUFFERSUM]    = { 0, 0, 0, 0, 0 };
static volatile uint16_t Bottom[RINGBUFFERSUM] = { 0, 0, 0, 0, 0 };
static volatile int32_t  Size[RINGBUFFERSUM]   = { 0, 0, 0, 0, 0 };
static volatile uint8_t  Status[RINGBUFFERSUM] = { BUFFEREMPTY, BUFFEREMPTY, BUFFEREMPTY,BUFFEREMPTY, BUFFEREMPTY};

/* Private functions ---------------------------------------------------------*/
static void    update_read_status( uint8_t sel) ;
static void    update_write_status( uint8_t sel) ;
static void    update_top( uint8_t sel, uint8_t Len );
static void    update_bottom( uint8_t sel, uint8_t Len );
static uint8_t get(uint8_t sel, uint16_t index);
static void    set(uint8_t sel, uint16_t index,uint8_t data);

/******************************************************************************
  Function:buffer_get_buffer_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t buffer_get_buffer_status( uint8_t sel )
{
	return Status[sel];
}

/******************************************************************************
  Function:get
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t get( uint8_t sel, uint16_t index )
{
	uint8_t data = pBuffer[sel][index % BufferSize[sel]];
	pBuffer[sel][index % BufferSize[sel]] = 0;
	return  data;
}

/******************************************************************************
  Function:set
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void set( uint8_t sel, uint16_t index, uint8_t data)
{
	pBuffer[sel][index % BufferSize[sel]] = data;
}


/******************************************************************************
  Function:update_read_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_read_status( uint8_t sel)
{
	uint8_t bufferstatus = buffer_get_buffer_status(sel);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			break;
		case BUFFERUSEING:
			{
				if(Size[sel] > 0)
					Status[sel] = BUFFERUSEING;
				else if(Size[sel] == 0)
					Status[sel] = BUFFEREMPTY;
				else
				{
					Status[sel] = BUFFEREMPTY;
					Top[sel]    = 0;
					Bottom[sel] = 0;
				}
			}
			break;
		case BUFFERFULL:
			Status[sel] = BUFFERUSEING;
			break;

		default:
			break;
	}
}

/******************************************************************************
  Function:update_write_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_write_status( uint8_t sel)
{
	uint8_t bufferstatus = buffer_get_buffer_status(sel);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			Status[sel] = BUFFERUSEING;
			break;

		case BUFFERUSEING:
			{
				if( Size[sel] > BufferSize[sel]-PACKETSIZE )
					Status[sel] = BUFFERFULL;
				else
					Status[sel] = BUFFERUSEING;
			}
			break;

		default:
			break;
	}
}
/******************************************************************************
  Function:update_top
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_top( uint8_t sel, uint8_t Len )
{
	Size[sel] += Len;
	Top[sel] = (Top[sel] + Len) % BufferSize[sel];
}

/******************************************************************************
  Function:update_bottom
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_bottom( uint8_t sel, uint8_t Len )
{
	Size[sel] -= Len;
	Bottom[sel] = (Bottom[sel] + Len) % BufferSize[sel];
}

/******************************************************************************
  Function:serial_ringbuffer_write_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_write_data(uint8_t sel, Uart_MessageTypeDef *message)
{
	uint8_t i;
	uint8_t *pdata = (uint8_t *)message;
	uint16_t MessageLen = *(uint16_t *)(message->LEN) + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA;

	for(i=0;i<=MessageLen;i++)
	{
		set(sel,Top[sel]+i,*pdata);
		pdata++;
	}

	set(sel,Top[sel]+i+0,message->XOR);
	set(sel,Top[sel]+i+1,message->END);

	update_top( sel, MessageLen+3);
	update_write_status(sel);
}
/******************************************************************************
  Function:serial_ringbuffer_read_data
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void serial_ringbuffer_read_data( uint8_t sel, Uart_MessageTypeDef *message )
{
		uint8_t i;
	  uint8_t *pdata = (uint8_t *)message;

	  uint16_t MessageLen = get( sel,Bottom[sel]+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA-1) +
												  get( sel,Bottom[sel]+MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA)*256
	                        + MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA;

		for(i=0;i<=MessageLen;i++)
		{
			*pdata = get(sel,Bottom[sel]+i);
			pdata++;
		}
		*(uint16_t *)(message->LEN) = MessageLen - MESSAGE_DATA_LEN_FROM_DEVICE_TO_DATA;
		message->XOR = get(sel,Bottom[sel]+i+0);
		message->END = get(sel,Bottom[sel]+i+1);

		update_bottom(sel, MessageLen+3);
		update_read_status(sel);
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		spi RF 消息缓存处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_write_data_to_buffer( uint8_t sel, uint8_t SpiMessage[], uint8_t status )
{
	uint8_t Len, *pdata, i;

	Len = SpiMessage[14];
	pdata = SpiMessage;

	for(i=0;i<Len+17;i++)
	{
		set(sel,Top[sel]+i,*pdata);
		pdata++;
	}

	set(sel,Top[sel]+i++,status);

	update_top( sel, Len+17+1);
	update_write_status(sel);
}

/******************************************************************************
  Function:App_rf_check_process
  Description:
		spi RF 消息读取处理函数
  Input :
  Return:
  Others:None
******************************************************************************/
void spi_read_data_from_buffer( uint8_t sel, uint8_t SpiMessage[] )
{
	uint8_t *pdata, i;
	uint8_t MessageLen = get( sel,Bottom[sel]+14) + 17;

	pdata = SpiMessage;

	for(i=0;i<=MessageLen;i++)
	{
		*pdata = get(sel,Bottom[sel]+i);
		pdata++;
	}

	SpiMessage[14] = MessageLen - 17;

	update_bottom(sel, MessageLen+1);
	update_read_status(sel);
}

/******************************************************************************
  Function:serial_ringbuffer_get_usage_rate
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t serial_ringbuffer_get_usage_rate(uint8_t sel)
{
	return (Size[sel]*100/BufferSize[sel]);
}

/******************************************************************************
  Function:print_write_data_to_buffer
  Description:
		存储打印数据到缓存
  Input :
  Return:
  Others:None
******************************************************************************/
void print_write_data_to_buffer( char *str, uint8_t len )
{
	char *pdata;
  uint8_t	i;

	pdata = str;

	for(i=0;i<len;i++)
	{
		set(PRINT_BUFFER,Top[PRINT_BUFFER]+i,*pdata);
		pdata++;
	}

	update_top( PRINT_BUFFER, len);
	update_write_status(PRINT_BUFFER);
}

/******************************************************************************
  Function:print_write_data_to_buffer
  Description:
		从缓存中读取打印数据
  Input :
  Return:
  Others:None
******************************************************************************/
void print_read_data_to_buffer( uint8_t *str ,uint8_t size)
{
	uint8_t Len, *pdata, i;

	Len = Size[PRINT_BUFFER] > size ? size : Size[PRINT_BUFFER];
	pdata = str;

	for(i=0;i<Len;i++)
	{
		*pdata = get(PRINT_BUFFER,Bottom[PRINT_BUFFER]+i);
		pdata++;
	}

	update_bottom(PRINT_BUFFER, Len);
	update_read_status(PRINT_BUFFER);
}

/**************************************END OF FILE****************************/
