#include "my_spi.h"

//#define SPI_DEBUG

#ifdef SPI_DEBUG
#define spi_debug  printf   
#else  
#define spi_debug(...)                    
#endif 

uint8_t tx_data_len;
uint8_t m_tx_buf[TX_BUF_SIZE];   /**< SPI TX buffer. */      
uint8_t m_rx_buf[RX_BUF_SIZE];   /**< SPI RX buffer. */ 
uint8_t flag_m_tx_buf_not_empty = 0;
volatile uint8_t	rf_int_status = 0;
uint8_t spi_status = 0;
static uint8_t is_valid_data = 0;

static const nrf_drv_spis_t spis = NRF_DRV_SPIS_INSTANCE(1);

/**@brief Function for initializing buffers.
 *
 * @param[in] p_tx_buf  Pointer to a transmit buffer.
 * @param[in] p_rx_buf  Pointer to a receive  buffer.
 * @param[in] len       Buffers length.
 */
static __INLINE void spi_slave_buffers_init(uint8_t * const p_tx_buf, uint8_t * const p_rx_buf, const uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        p_tx_buf[i] = (uint8_t)0x00;
        p_rx_buf[i] = 0;
    }
}

/**@brief Function for initializing buffers.
 *
 * @param[in] p_tx_buf  Pointer to a transmit buffer.
 * @param[in] p_rx_buf  Pointer to a receive  buffer.
 * @param[in] len       Buffers length.
 */
static __INLINE void spi_slave_tx_buffers_init(uint8_t * const p_tx_buf, const uint16_t len)
{
    uint16_t i;
    for (i = 0; i < len; i++)
    {
        p_tx_buf[i] = (uint8_t)0x66;
    }
}

//SPI相关引脚初始化
void spi_gpio_init(void)
{
	nrf_gpio_cfg_output(SPIS_IRQ_PIN);
	nrf_gpio_cfg_input(SPIS_CE_PIN, NRF_GPIO_PIN_PULLUP);  
	nrf_gpio_pin_set(SPIS_IRQ_PIN);
}

// SPI格式
// byte0 UESB_WRITE_TX_PAYLOAD
// byte1 len
// byte2 re_tx_num
// byte3 re_tx_delay
// byte4-... 0x61-...-0x21
// byte_end xor
uint8_t switch_flag = 1;
void spi_data_handler(uint8_t *p_src, uint8_t len)
{
	uint8_t i,j;
	uint8_t temp_data[TX_BUF_SIZE] = {0x00}; 
	memcpy(temp_data, p_src, len);						
	memset(p_src, 0x00, len);							
	
//	spi_debug("spi rx len:%d \r\n",len);
//	for( i = 0 ;i < len ;i++)
//	{
//		spi_debug("%02X ",temp_data[i]);
//	}spi_debug("\r\n");
	
	/* 收到数据且异或校验正确，则存入缓冲区等待处理 */
	if(len > 2 && (temp_data[len - 1] == XOR_Cal(temp_data, len - 1)))			
	{
		if( UESB_WRITE_TX_PAYLOAD == temp_data[0] )
		{
			if(temp_data[4] == 0x61 && temp_data[len-2] == 0x21)
			{
				if(get_ringbuf_status() != BUFF_FULL)
				{
					ringbuf_write_data(&temp_data[2],temp_data[1]); //老版本 只存data值
				}
				else
				{
					spi_debug("spi ringbuf full \r\n");
				}
			}
			else
			{
				spi_debug("spi rx format err \r\n");
			}
		}
	}
	else
	{
		spi_debug("spi rx check err \r\n");
	}
}

void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    { 
		//spi_debug("spi_debug:spi trans down.   \r\n");
		nrf_gpio_pin_set(RX_PIN_NUMBER_1);		//debug
		spi_status = 0;
		spi_data_handler(m_rx_buf, event.rx_amount);
		
		if(is_valid_data && (!flag_m_tx_buf_not_empty))			//APDU数据存在时不初始化buffer，防止有效数据被清除
		{
			rf_int_status = 0;
			is_valid_data = 0;
			spi_slave_tx_buffers_init(m_tx_buf, tx_data_len);
		}
		nrf_drv_spis_buffers_set(&spis,m_tx_buf,TX_BUF_SIZE,m_rx_buf,RX_BUF_SIZE);
    }
}


void my_spi_slave_init(void)
{
    uint32_t err_code;
	ret_code_t err;
    nrf_drv_spis_config_t spis_config = NRF_DRV_SPIS_DEFAULT_CONFIG(1);              
    spis_config.miso_pin = SPIS_MISO_PIN;                                         
    spis_config.mosi_pin = SPIS_MOSI_PIN;                                         
    spis_config.sck_pin = SPIS_SCK_PIN;
    spis_config.csn_pin	= SPIS_CSN_PIN;
    spis_config.mode = NRF_DRV_SPIS_MODE_0;
    spis_config.bit_order = NRF_DRV_SPIS_BIT_ORDER_MSB_FIRST;
    spis_config.def = NRF_DRV_SPIS_DEFAULT_DEF;
    spis_config.orc = NRF_DRV_SPIS_DEFAULT_ORC;
	
    APP_ERROR_CHECK(nrf_drv_spis_init(&spis, &spis_config, spis_event_handler));
	
	spi_slave_buffers_init(m_tx_buf, m_rx_buf, (uint16_t)TX_BUF_SIZE);
	APP_ERROR_CHECK(nrf_drv_spis_buffers_set(&spis,m_tx_buf,TX_BUF_SIZE,m_rx_buf,RX_BUF_SIZE));
}











