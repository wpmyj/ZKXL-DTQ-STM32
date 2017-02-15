#ifndef __APP_SPI_SEND_DATA_PROCESS_H_
#define __APP_SPI_SEND_DATA_PROCESS_H_

#include "main.h"
#include "app_show_message_process.h"

//#define ENABLE_SPI_DEBUG_LOG

#ifdef 	ENABLE_SPI_DEBUG_LOG
#define DEBUG_SPI_LOG							     DEBUG_LOG
#else
#define DEBUG_SPI_LOG(...)
#endif

#define SPI_SEND_DATA_PRE_DELAY        10
#define SPI_SEND_DATA_BUFFER_COUNT_MAX 3

void change_spi_send_data_process_status( uint8_t new_status );
uint8_t get_spi_send_data_process_status( void );
void App_spi_send_data_process(void);
void spi_send_data_process_timer_init( void );
void spi_send_data_write_tx_payload( uint8_t *tx_buf,uint8_t tx_buf_len, uint8_t tx_count,
	                         uint8_t nrf51822_delay100us, uint8_t spi_delayms );
#endif
