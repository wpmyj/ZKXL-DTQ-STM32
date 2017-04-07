#include "define.h"

static void debug_fun(void);

int main (void)
{
	GPIO_default();		
	CLOCK_Init();
	TIMER_init();
	KEY_init();
	POWER_init();
	LCD_init();   
	
	POWER_On();								//长按开机功能
	
	UART_init();							//UART不初始化能降功耗
	debug_logic_init();	
	NFC_Init();								
	RTC_Init();
	ADC_init();
	RADIO_init();
	PARAMETER_init();
	APP_init();								// APP_init要放在NFC后,因为NFC读取UID要在APP_init中使用 		

	TIMER_RxWindowStart();
	TIMER_LcdUpdateStart();
	POWER->sys_state = SYS_ON;
	POWER->request_state = SYS_ON;
	TIMER_SysSleepStart();				
	
	LCD_display_student_id(STUDENT_ID);			
	while(true)
	{
//		debug_fun();       					//调试用函数
		switch (POWER->sys_state)
        {
        	case SYS_ON:  
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLK_HFCLKStart();
					RADIO_Wakeup();
//					LCD_display_wakeup();				//LCD唤醒放在POWER唤醒函数里，避免产生视觉延时			
					POWER->request_state = SYS_SLEEP;	
					POWER->request_data_flg = false;	
					TIMER_SysSleepStart();				//30S无按键进入休眠状态
					TIMER_SysOffStop();					//45min无按键自动关机
				}
				TIMER_MyEventHandler();				
				RADIO_RxDataHandler();				
				APP_CmdHandler();			
				APP_KeyHandler();
				RTC->update();										
				ADC->update();
				LCD_display_update();	 
				break;
        	case SYS_SLEEP: 
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					HFCLK_stop();			
					LCD_display_sleep();			
					RADIO_Sleep();
					POWER->sys_off_count = 0;
					TIMER_SysOffStart();		//待机超时自动关机定时器开启
				}
				TIMER_MyEventHandler();
				APP_KeyHandler();
        		break;
        	default:
        		break;
        }
		__WFE();							//POWER DOWN，wait for EVENT	
		__WFI();
	}
}

static void debug_fun(void)
{
	logic_high();
	nrf_delay_ms(20);
	logic_low();
	nrf_delay_ms(20);
}










