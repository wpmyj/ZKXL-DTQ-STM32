#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "app_error.h"
#include "nrf_delay.h"
#include "simple_uart.h"
#include "my_uart.h"
#include "my_misc_fun.h"
#include "my_radio.h"
#include "my_timer.h"
#include "ringbuf2.h"



int main (void)
{
    ret_code_t err_code;
	
	clocks_start();
	debug_uart_init();			//别忘答题器和接收器串口脚不一样
	nrf_gpio_cfg_output(RX_PIN_NUMBER_1);	//用于示波器测量电平跳变
	timers_init();
//	nrf_delay_ms(500);			//测试使用，上电抖动延时
	spi_gpio_init();
	my_spi_slave_init();

    err_code = my_rx_esb_init();
    APP_ERROR_CHECK(err_code);
    err_code = nrf_esb_start_rx();
    APP_ERROR_CHECK(err_code);
	
	temp_timeout_start();			//RTC校准定时器

	
	while(true)
	{
		if(spi_busy_status == SPI_IDLE)
		{
//			if((get_ringbuf_status() != BUFF_EMPTY))
//			{
//				read_ringbuf_use_num();
				nrf_rx_data_handler();
//			}	
		}
//		printf("main running \r\n");
	}
}



