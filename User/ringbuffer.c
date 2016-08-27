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
static uint8_t spiringbuffer[SPIRINGBUFFERSIZE][PACKETSIZE];
static uint8_t readindex = 0;
static uint8_t writeindex = 0;
static uint8_t ringbufferstatus = BUFFEREMPTY;

/* Private functions ---------------------------------------------------------*/
static void ReadChangeBufferSattus(void) ;
static void WriteChangeBufferSattus(void) ;
static void UpdateBufferWriteIndex( void );
static void UpdateBufferReadIndex( void );
static void ClearBufferElement(uint8_t index);

/******************************************************************************
  Function:GetBufferStatus
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t GetBufferStatus(void)
{
	return ringbufferstatus;
}

/******************************************************************************
  Function:GetBufferReadIndex
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t GetBufferReadIndex(void)
{
	return readindex;
}

/******************************************************************************
  Function:GetBufferWriteIndex
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
uint8_t GetBufferWriteIndex(void)
{
	return writeindex;
}

/******************************************************************************
  Function:ReadChangeBufferSattus
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void ReadChangeBufferSattus(void) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = GetBufferStatus();

	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			break;
		
		case BUFFERUSEING:
			{
				if(readindex == writeindex)
					ringbufferstatus = BUFFEREMPTY;
				else
					ringbufferstatus = BUFFERUSEING;
			}
			break;
			
		case BUFFERFULL:
			{
					ringbufferstatus = BUFFERUSEING;
			}
			break;
			
		default:
			break;
	}
}

/******************************************************************************
  Function:WriteChangeBufferSattus
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void WriteChangeBufferSattus(void) 
{
	uint8_t bufferstatus = 0;
	
	bufferstatus = GetBufferStatus();
	
	switch(bufferstatus)
	{
		case BUFFEREMPTY:
			{
					ringbufferstatus = BUFFERUSEING;
			}
			break;
		
		case BUFFERUSEING:
			{
				if(writeindex == readindex)
					ringbufferstatus = BUFFERFULL;
				else
					ringbufferstatus = BUFFERUSEING;
			}
			break;
			
		case BUFFERFULL:
			break;
			
		default:
			break;
	}
	
}
/******************************************************************************
  Function:UpdateBufferWriteIndex
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void UpdateBufferWriteIndex( void )
{
	writeindex++;
	if(writeindex>=SPIRINGBUFFERSIZE)
		writeindex = 0;
}

/******************************************************************************
  Function:UpdateBufferReadIndex
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void UpdateBufferReadIndex( void )
{
	readindex++;
	if(readindex>=SPIRINGBUFFERSIZE)
		readindex = 0;
}

/******************************************************************************
  Function:ClearBufferElement
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
static void ClearBufferElement(uint8_t index)
{
	uint8_t *pdata;
	uint8_t i;
	
	pdata = spiringbuffer[index];

	for(i=0;i<PACKETSIZE;i++)
	{
		*(pdata) = 0;
		pdata++;
	}
}

/******************************************************************************
  Function:RingBufferWriteData
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void RingBufferWriteData(uint8_t data[])
{
	uint8_t *pdata;
	uint8_t i;
	uint8_t bufferindex;
	
	pdata = data;
	bufferindex = GetBufferWriteIndex();
	
	for(i=0;i<PACKETSIZE;i++)
	{
		spiringbuffer[bufferindex][i] = *(pdata);
		pdata++;
	}
	
	UpdateBufferWriteIndex();
	
	WriteChangeBufferSattus();
}

/******************************************************************************
  Function:RingbufferReadData
  Description:
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void RingbufferReadData(uint8_t data[])
{
		uint8_t *pdata;
		uint8_t i;
		uint8_t bufferindex;

		pdata = data;
		bufferindex = GetBufferReadIndex();
	
		for(i=0;i<PACKETSIZE;i++)
		{
			*(pdata) = spiringbuffer[bufferindex][i];
			pdata++;
		}
		
		ClearBufferElement(bufferindex);
		
		UpdateBufferReadIndex();
		
		ReadChangeBufferSattus();
}
