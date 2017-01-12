#include "my_spi.h"


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


void spis_event_handler(nrf_drv_spis_event_t event)
{
    if (event.evt_type == NRF_DRV_SPIS_XFER_DONE)
    { 
		//printf("spi_debug:spi trans down.   \r\n");
		spi_status = 0;
		//spi_data_handler(m_rx_buf, event.rx_amount);
		
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











