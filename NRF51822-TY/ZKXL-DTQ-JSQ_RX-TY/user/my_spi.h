#ifndef _MY_SPI_H
#define _MY_SPI_H

#include "nrf_drv_spis.h"
#include "nrf_drv_config.h"
#include "nrf_gpio.h"
#include "my_radio.h"
#include "nrf_error.h"
#include "my_timer.h"



#define TX_BUF_SIZE   (255)           	  /**< SPI TX buffer size. */      
#define RX_BUF_SIZE   TX_BUF_SIZE       /**< SPI RX buffer size. */  

#define SPI_BUSY	(1)
#define SPI_IDLE	(0)



#define SPIS_IRQ_PIN   0     
#define SPIS_CE_PIN    1
#define SPIS_MOSI_PIN  2    // SPI MOSI signal. 
#define SPIS_MISO_PIN  3    // SPI MISO signal. 
#define SPIS_SCK_PIN   4    // SPI SCK signal.
#define SPIS_CSN_PIN   5    // SPI CSN signal. 

typedef enum {
    UESB_WRITE_PARAM 				= 0x80,
    UESB_FLUSH_TX 					= 0x81,
    UESB_FLUSH_RX 					= 0x82,
	UESB_WRITE_TX_PAYLOAD 			= 0x83,
	UESB_WRITE_TX_PAYLOAD_NOACK 	= 0x84,
	UESB_READ_RX_PAYLOAD 			= 0x85,
	UESB_READ_RF_INT_STATUS 		= 0x86,
	UESB_CLEAR_RF_INT_STATUS 		= 0x87,
	UESB_SWITCH_TX_RX	 			= 0x88,
} spi_cmd_t;

extern uint8_t tx_data_len;
extern uint8_t m_tx_buf[TX_BUF_SIZE];   /**< SPI TX buffer. */      
extern uint8_t m_rx_buf[RX_BUF_SIZE];   /**< SPI RX buffer. */ 
extern uint8_t flag_m_tx_buf_not_empty;
extern volatile uint8_t	rf_int_status;
extern uint8_t spi_status;
extern uint8_t spi_busy_status;


void my_spi_slave_init(void);
void spi_gpio_init(void);
	
#endif


