

/*-------------------------------- Include File -----------------------------------------*/
#include "power.h"
/*-------------------------------- Macro definition -------------------------------------*/

/*-------------------------------- Local Variable ----------------------------------------*/

power_struct_t				power_struct;

/*-------------------------------- Gocal Variable ----------------------------------------*/
power_struct_t				*POWER = &power_struct;
/*-------------------------------- Function Declaration ----------------------------------*/



/*----------------------------------------------------------------------------------------*/

void POWER_init(void)
{
	
	/* 结构体参数初始化 */
	POWER->sys_state = SYS_OFF;
	POWER->state_change_flg = false;
	POWER->request_data_flg = false;
	POWER->request_data_num = 0x00;
	POWER->sys_off_count    = 0;
	POWER->KeyOnIngFlg = false;
	
}

void POWER_Off(void)
{
	LCD_display_sleep();
	
	/* 系统开机，才能发送关机状态 */
	if((POWER->sys_state == SYS_ON) || (POWER->sys_state == SYS_SLEEP))
	{
//		send_system_state(SYS_OFF);	
//		nrf_delay_ms(15);
	}	
	GPIO_default();							//关机前GPIO配置为高阻态，防止其他按键唤醒系统
	nrf_gpio_cfg_sense_input(KEY_PWR, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);			
	NRF_POWER->SYSTEMOFF = 1;				//进入休眠模式
}


void POWER_On(void)
{
	if(0x00 == nrf_gpio_pin_read(KEY_PWR))			//按键按下拉低为0		
	{
		TIMER_PowerOnStart();		//软件关机改为定时器，降功耗
		while(false == POWER->KeyOnIngFlg)
		{
			__WFE();				
			__WFI();
		}
		POWER->KeyOnIngFlg = false;
//		nrf_delay_ms(2000);
		if(nrf_gpio_pin_read(KEY_PWR))
		{
			POWER_Off();
		}
		else
		{	
			LCD_diplay_on();
		}
	}
	else
		POWER_Off();
}


void POWER_Sleep(void)
{
	/* 系统状态切换为SLEEP */
	POWER->state_change_flg = true;	
	POWER->sys_state = SYS_SLEEP;
		
}

void POWER_Wakeup(void)
{
	/* 系统状态切换为ON */
	POWER->state_change_flg = true;	
	POWER->sys_state = SYS_ON;
	LCD_display_wakeup();	
		
}


void POWER_DCDCEnable(void)
{
	NRF_POWER->DCDCEN = 1;
}

void POWER_DCDCDisable(void)
{
	NRF_POWER->DCDCEN = 0;
}

















