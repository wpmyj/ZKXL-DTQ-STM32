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
#include "app_serial_cmd_process.h"

/* Private define ------------------------------------------------------------*/
/* Set parameter of budffer */
#define UART_RBUF            0
#define UART_SBUF            1
#define PRINT_BUF            2
#define SPI_RBUF             3

#define PACKETSIZE           (UART_NBUF+9)
#define UART_RBUF_SIZE       (1)
#define UART_SBUF_SIZE       (1)
#define SPI_RBUF_SIZE        (1024*2)
#define PRINT_RBUF_SIZE      (1024*2)
#define BUF_NUM              (4)


/* buffer status  */
#define BUF_EMPTY            1
#define BUF_USEING           2
#define BUF_FULL             3

/* the threshold of buffer */
#define USAGE_TATE_FULL      (100-PACKETSIZE*100/UART_RBUF_SIZE)
#define USAGE_TATE_EREMPTY   0

/* buffer operation */
#define OPERATIONWRITE       1
#define OPERATIONREAD        2

#define CLOSEIRQ()           __set_PRIMASK(1)        
#define OPENIRQ()            __set_PRIMASK(0)

#define SPI_DATA_IRQ_BUFFER_BLOCK_COUNT          4

/* Private functions ---------------------------------------------------------*/
uint8_t buffer_get_buffer_status( uint8_t sel );
//void serial_ringbuffer_write_data( uint8_t sel, Uart_MessageTypeDef *data );
//void serial_ringbuffer_read_data( uint8_t sel, Uart_MessageTypeDef *data );
uint8_t serial_ringbuffer_get_usage_rate(uint8_t sel);
void spi_read_data_from_buffer( uint8_t sel,uint8_t SpiMessage[] );
void spi_write_data_to_buffer( uint8_t sel, uint8_t SpiMessage[], uint8_t status );
void print_write_data_to_buffer( char *str, uint8_t len );
void print_read_data_to_buffer( uint8_t *str ,uint8_t size);
#endif
