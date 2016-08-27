/**
  ******************************************************************************
  * @file   	ringbuffer.h
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	ringbuffer
  ******************************************************************************
  */
	
#ifndef __RINGBUFFER_H_
#define __RINGBUFFER_H_

#include "stm32f10x.h"

/* Private define ------------------------------------------------------------*/
/* Set parameter of budffer */
#define SPIRINGBUFFERSIZE    10
#define PACKETSIZE           250

/* buffer status  */
#define BUFFEREMPTY          1
#define BUFFERUSEING         2
#define BUFFERFULL           3

/* buffer operation */
#define OPERATIONWRITE       1
#define OPERATIONREAD        2

#define CLOSEIRQ()           __set_PRIMASK(1)        
#define OPENIRQ()            __set_PRIMASK(0)

/* Private functions ---------------------------------------------------------*/
uint8_t GetBufferStatus(void);
uint8_t GetBufferReadIndex(void);
uint8_t GetBufferWriteIndex(void);
void RingBufferWriteData(uint8_t data[]);
void RingbufferReadData(uint8_t data[]);

#endif