#include "my_wdt.h"

nrf_drv_wdt_channel_id m_channel_id;



void wdt_event_handler(void)
{
	printf("WDT \r\n");
}


void my_wdt_init(void)
{
	uint32_t err_code = NRF_SUCCESS;
	
    nrf_drv_wdt_config_t config = NRF_DRV_WDT_DEAFULT_CONFIG;
    err_code = nrf_drv_wdt_init(&config, wdt_event_handler);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_wdt_channel_alloc(&m_channel_id);
    APP_ERROR_CHECK(err_code);
    nrf_drv_wdt_enable();
}



















