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
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

//#include "stm32f10x.h"
//#include "pos_handle_layer.h"


/* Private define ------------------------------------------------------------*/
/* Set parameter of budffer */
#define REVICE_RINGBUFFER    0
#define SEND_RINGBUFFER      1

//#define PACKETSIZE           (UART_NBUF+9)
//#define REVICEBUFFERSIZE     (2048)
//#define SENDBUFFERSIZE       (2048)
#define PACKETSIZE           (1)
#define REVICEBUFFERSIZE     (1)
#define SENDBUFFERSIZE       (1)

/* buffer status  */
#define BUFFEREMPTY          1
#define BUFFERUSEING         2
#define BUFFERFULL           3

/* the threshold of buffer */
#define USAGE_TATE_FULL      (100-PACKETSIZE*100/REVICEBUFFERSIZE)
#define USAGE_TATE_EREMPTY   0

/* buffer operation */
#define OPERATIONWRITE       1
#define OPERATIONREAD        2

#define CLOSEIRQ()           __set_PRIMASK(1)        
#define OPENIRQ()            __set_PRIMASK(0)

/* Private functions ---------------------------------------------------------*/
uint8_t buffer_get_buffer_status( uint8_t sel_buffer );
//void serial_ringbuffer_write_data( uint8_t sel_buffer, Uart_MessageTypeDef *data );
//void serial_ringbuffer_read_data( uint8_t sel_buffer, Uart_MessageTypeDef *data );
void serial_ringbuffer_write_data1(uint8_t sel_buffer, uint8_t *data, uint8_t len);
void serial_ringbuffer_read_data1( uint8_t sel_buffer, uint8_t *data, uint8_t len);
uint8_t serial_ringbuffer_get_usage_rate(uint8_t sel_buffer);
#endif
