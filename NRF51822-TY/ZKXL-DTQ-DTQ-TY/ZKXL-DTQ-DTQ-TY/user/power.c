

/*-------------------------------- Include File -----------------------------------------*/
#include "power.h"
/*-------------------------------- Macro definition -------------------------------------*/

/*-------------------------------- Local Variable ----------------------------------------*/

power_struct_t				power_struct;

/*-------------------------------- Gocal Variable ----------------------------------------*/
power_struct_t				*POWER = &power_struct;
/*-------------------------------- Function Declaration ----------------------------------*/

static void power_off(void);
static void power_on(void);
static void power_sleep(void);
static void power_wakeup(void);
static void send_system_state(System_State sys_off_on);

/*----------------------------------------------------------------------------------------*/

void POWER_init(void)
{
	
	/* 结构体参数初始化 */
	POWER->sys_state = SYS_OFF;
	POWER->state_change_flg = false;
	POWER->request_data_flg = false;
	POWER->request_data_num = 0x00;
	POWER->sys_off_count    = 0;
	
	POWER->off 		= power_off;
	POWER->on 		= power_on;
	POWER->sleep	= power_sleep;
	POWER->wakeup	= power_wakeup;
	
	POWER->send_state = send_system_state;
}

void power_off(void)
{
	LCD->sleep();
	
	/* 系统开机，才能发送关机状态 */
	if((POWER->sys_state == SYS_ON) || (POWER->sys_state == SYS_SLEEP))
	{
		send_system_state(SYS_OFF);	
		nrf_delay_ms(15);
	}	
	GPIO_default();							//关机前GPIO配置为高阻态，防止其他按键唤醒系统
	nrf_gpio_cfg_sense_input(KEY_PWR, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);			
	NRF_POWER->SYSTEMOFF = 1;				//进入休眠模式
}


void power_on(void)
{
	if(0x00 == nrf_gpio_pin_read(KEY_PWR))			//按键按下拉低为0		
	{
		TIMER->power_on_start();		//软件关机改为定时器，降功耗
		__WFE();				
		__WFI();
//		nrf_delay_ms(2000);
		if(nrf_gpio_pin_read(KEY_PWR))
		{
			power_off();
		}
		else
		{	
			LCD->on();
		}
	}
	else
		power_off();
}


void power_sleep(void)
{
	/* 系统状态切换为SLEEP */
	POWER->state_change_flg = true;	
	POWER->sys_state = SYS_SLEEP;
		
}

void power_wakeup(void)
{
	/* 系统状态切换为ON */
	POWER->state_change_flg = true;	
	POWER->sys_state = SYS_ON;
		
}


void send_system_state(System_State sys_off_on)
{
	uint8_t i;
	
	TRANSPORT.data_len = 28;
	
	TRANSPORT.data[9]++;
	TRANSPORT.data[10]++;
	TRANSPORT.data[14] = 0x0B;				//length
	TRANSPORT.data[15] = 0x5A;
	TRANSPORT.data[16] = DTQ_UID[0];
	TRANSPORT.data[17] = DTQ_UID[1];
	TRANSPORT.data[18] = DTQ_UID[2];
	TRANSPORT.data[19] = DTQ_UID[3];
	TRANSPORT.data[20] = 0x00;				//reserve
	TRANSPORT.data[21] = 0x14;
	TRANSPORT.data[22] = 0x01;
	TRANSPORT.data[23] = sys_off_on;
	TRANSPORT.data[24] = XOR_Cal(TRANSPORT.data+16,8);
	TRANSPORT.data[25] = 0xCA;
	TRANSPORT.data[26] = XOR_Cal(TRANSPORT.data+1,25);
	TRANSPORT.data[27] = 0x21;

	
	/* 每次发送前提取要发送的数据，保证tx_payload.data结构体中的数据是最新的 */
	tx_payload.length = TRANSPORT.data_len;
	tx_payload.pipe    = NRF_PIPE;
	tx_payload.noack = true;
	memcpy(tx_payload.data, TRANSPORT.data, tx_payload.length);
	
//	tmp_debug("tx_len:%d \r\n",tx_payload.length);
//	for(i = 0;i < tx_payload.length;i++)
//		tmp_debug("%02X ",tx_payload.data[i]);
//	tmp_debug("\r\n");
	
	/* 启动发送配置 */
//	CLOCK->HFCLK_start();				//发送前开启外部时钟
	TRANSPORT.sequence_re_tx_num = 1;
	TIMER->rx_window_stop();	 			
	TIMER->tx_overtime_start(); 			 
	my_esb_mode_change(NRF_ESB_MODE_PTX);
	nrf_esb_flush_tx();
	nrf_esb_write_payload(&tx_payload);	
}
