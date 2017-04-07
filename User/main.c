
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

//#define MAIN_DEBUG

#ifdef MAIN_DEBUG
#define main_debug  printf   
#else  
#define main_debug(...)                    
#endif 

static void spi_rx_data_handler(void);

int main (void)
{
    ret_code_t err_code;
	
	nrf_delay_ms(500);			//测试使用，上电抖动延时
	
	nrf_gpio_cfg_output(RX_PIN_NUMBER_1);
	
	clocks_start();
//	debug_uart_init();			//别忘答题器和接收器串口脚不一样
	timers_init();
	spi_gpio_init();
	my_spi_slave_init();

    err_code = my_tx_esb_init();
    APP_ERROR_CHECK(err_code);

	temp_timeout_start();			//RTC校准定时器
	tx_payload.noack  = true;
	tx_payload.pipe   = NRF_PIPE;
	tx_ing_flag = false;
	nrf_tx_flg = false;
	
	while(true)
	{
		if(true == tx_ing_flag)					//发送系统空闲
		{			
			if(true == nrf_tx_flg)			//发送延时定时器超时
			{
				if(nrf_tx_num > 0)
				{
					nrf_esb_flush_tx();
					sequence_re_tx_num = 1;
					nrf_esb_write_payload(&tx_payload);
					nrf_tx_flg = false;	
					nrf_tx_num--;
					
					if(0x02 == tx_payload.data[11])		//若发送的是前导帧，更新发送序号
					{
						preamble_num++;					
						tx_payload.data[9] = preamble_num;
						tx_payload.data[31] = XOR_Cal(&tx_payload.data[1],30);	//重新校验
					}		
					
					if(0 == nrf_tx_num)
					{
						tx_ing_flag = false;
						nrf_transmit_timeout_stop();
					}
				}
			}
		}
		else if(false == tx_ing_flag)
		{
			if((get_ringbuf_status() != BUFF_EMPTY))
			{
				spi_rx_data_handler();
			}
		}
//		nrf_delay_ms(2);
//		printf("~~~~~~~~~~ \r\n");
	}
}


void spi_rx_data_handler(void)
{
	uint8_t i;
	uint8_t tmp_ringbuf_len = 0;
	uint8_t tmp_ringbuf_buf[250];

	ringbuf_read_data(tmp_ringbuf_buf,&tmp_ringbuf_len);
	
	memcpy(tx_payload.data,tmp_ringbuf_buf+2,tmp_ringbuf_len-2);
	tx_payload.length = tmp_ringbuf_len-2;
	nrf_tx_num = tmp_ringbuf_buf[0];						//发送总次数	
	nrf_tx_delay = tmp_ringbuf_buf[1];						//重发延时,100us倍数	
	
	main_debug("len:%d \r\n",tx_payload.length);
	
	/* 数据需要发送，置标志位并开启延时定时器 */
	if(nrf_tx_num > 0)
	{
		tx_ing_flag = true;
		nrf_tx_flg = false;
		if(0x02 == tx_payload.data[11])		//若发送的是前导帧
		{
			preamble_num = 0;				//前导帧序号从0开始
			tx_payload.data[9] = preamble_num;
			tx_payload.data[31] = XOR_Cal(&tx_payload.data[1],30);	//重新校验
		}
		nrf_transmit_timeout_start(nrf_tx_delay/10);
	}

	
	//重发放在这里而不在中断原因，有delay，而2.4G中断优先级高，会导致SPI无法触发
//	for(i = 0;i < nrf_tx_num;i++)
//	{
//		nrf_delay_100us(nrf_tx_delay);
////		nrf_gpio_pin_toggle(RX_PIN_NUMBER_1);
//		nrf_esb_write_payload(&tx_payload);
//	}
}
