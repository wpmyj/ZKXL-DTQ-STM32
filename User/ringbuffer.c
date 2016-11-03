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
const uint32_t  BufferSize[RINGBUFFERSUM]      = {REVICEBUFFERSIZE,SENDBUFFERSIZE,SPIBUFFERSIZE};
static uint8_t *pBuffer[RINGBUFFERSUM]         = {UartReviceBuffer,UartSendBuffer,SpireviceBuffer};
static volatile uint16_t Top[RINGBUFFERSUM]    = { 0, 0, 0 };
static volatile uint16_t Bottom[RINGBUFFERSUM] = { 0, 0, 0 };
static volatile int32_t  Size[RINGBUFFERSUM]   = { 0, 0, 0 };
static volatile uint8_t  Status[RINGBUFFERSUM] = { BUFFEREMPTY, BUFFEREMPTY, BUFFEREMPTY};

/* Private functions ---------------------------------------------------------*/
static void    update_read_status( uint8_t sel_buffer) ;
static void    update_write_status( uint8_t sel_buffer) ;
static void    update_top( uint8_t sel_buffer, uint8_t Len );
static void    update_bottom( uint8_t sel_buffer, uint8_t Len );
static uint8_t pop(uint8_t sel_buffer, uint16_t index);
static void    push(uint8_t sel_buffer, uint16_t index,uint8_t data);

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
	return Status[sel_buffer];
}

/******************************************************************************
  Function:pop
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t pop( uint8_t sel_buffer, uint16_t index )
{
	uint8_t data = pBuffer[sel_buffer][index % BufferSize[sel_buffer]];
	pBuffer[sel_buffer][index % BufferSize[sel_buffer]] = 0;
	return  data;
}

/******************************************************************************
  Function:push
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void push( uint8_t sel_buffer, uint16_t index, uint8_t data)
{
	pBuffer[sel_buffer][index % BufferSize[sel_buffer]] = data;
}


/******************************************************************************
  Function:update_read_status
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_read_status( uint8_t sel_buffer)
{
	uint8_t bufferstatus = 0;

	bufferstatus = buffer_get_buffer_status(sel_buffer);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:                                          break;
		case BUFFERUSEING:
			{
				if(Size[sel_buffer] > 0)
					Status[sel_buffer] = BUFFERUSEING;
				else if(Size[sel_buffer] == 0)
					Status[sel_buffer] = BUFFEREMPTY;
				else
				{
					Status[sel_buffer] = BUFFEREMPTY;
					Top[sel_buffer] = 0;
					Bottom[sel_buffer] = 0;
				}
			}
			break;
		case BUFFERFULL:Status[sel_buffer] = BUFFERUSEING;break;
		default:                                                    break;
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
static void update_write_status( uint8_t sel_buffer)
{
	uint8_t bufferstatus = 0;

	bufferstatus = buffer_get_buffer_status(sel_buffer);

	switch(bufferstatus)
	{
		case BUFFEREMPTY:Status[sel_buffer] = BUFFERUSEING;break;

		case BUFFERUSEING:
			{
				if(serial_ringbuffer_get_usage_rate(sel_buffer) >= USAGE_TATE_FULL)
					Status[sel_buffer] = BUFFERFULL;
				else
					Status[sel_buffer] = BUFFERUSEING;
			}
			break;

		case BUFFERFULL:                                            break;
		default:                                                    break;
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
static void update_top( uint8_t sel_buffer, uint8_t Len )
{
	Size[sel_buffer] += Len;
	Top[sel_buffer] = (Top[sel_buffer] + Len) % BufferSize[sel_buffer];
}

/******************************************************************************
  Function:update_bottom
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void update_bottom( uint8_t sel_buffer, uint8_t Len )
{
	Size[sel_buffer] -= Len;
	Bottom[sel_buffer] = (Bottom[sel_buffer] + Len) % BufferSize[sel_buffer];
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
		push(sel_buffer,
			Top[sel_buffer]+i,*pdata);
		pdata++;
	}

	push(sel_buffer,
		Top[sel_buffer]+i+0,data->XOR);

	push(sel_buffer,
		Top[sel_buffer]+i+1,data->END);

	update_top( sel_buffer, MessageLen+3);

	update_write_status(sel_buffer);
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
		push(sel_buffer,
				Top[sel_buffer]+i,*pdata);
			pdata++;
	}

	push(sel_buffer,
				Top[sel_buffer]+i++,send_data_status);

	update_top( sel_buffer, Len+17+1);

	update_write_status(sel_buffer);
}

void spi_read_data_from_buffer( uint8_t sel_buffer, uint8_t SpiMessage[] )
{
	uint8_t *pdata, i;
	uint8_t MessageLen = pop( sel_buffer,
														Bottom[sel_buffer]+14) + 17; 

	pdata = SpiMessage;

	for(i=0;i<=MessageLen;i++)
	{
		*pdata = pop(sel_buffer,
								Bottom[sel_buffer]+i);
		push(sel_buffer,
				Bottom[sel_buffer]+i,0);
		pdata++;
	}
	
	SpiMessage[14] = MessageLen - 17;

	update_bottom(sel_buffer, MessageLen+1);

	update_read_status(sel_buffer);
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

	  uint8_t MessageLen = pop( sel_buffer,
														Bottom[sel_buffer]+6) + 6;

		for(i=0;i<=MessageLen;i++)
		{
			*pdata = pop(sel_buffer,
									Bottom[sel_buffer]+i);
			pdata++;
		}
		data->LEN = MessageLen - 6;

		data->XOR = pop(sel_buffer,
										Bottom[sel_buffer]+i+0);

		data->END = pop(sel_buffer,
										Bottom[sel_buffer]+i+1);

		update_bottom(sel_buffer, MessageLen+3);

		update_read_status(sel_buffer);
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

	usage_rate = Size[sel_buffer]*100/BufferSize[sel_buffer];

	return usage_rate;
}

/**************************************END OF FILE****************************/
