#include "define.h"

static void debug_fun(void);

int main (void)
{
//	nrf_delay_ms(10);						//上电延时	
	GPIO_default();
								
	CLOCK_init();
//	UART_init();							//UART不初始化能降功耗
	debug_logic_init();
	TIMER_init();
	LCD_init();  
	NFC_init();								//NRF需放在KEY_init()前面，因为他的中断脚在KEY_init()里配置了
	KEY_init();
	RTC_Init();
	ADC_init();
	RADIO_init();
	POWER_init();
	PARAMETER_init();
	APP_init();								// APP_init要放在NFC后,因为NFC读取UID要在APP_init中使用 		
		
	POWER->on();							//长按开机，要放在APP_init后，
	
	TIMER->rx_window_start();
	LCD->timer_start();
//	LCD->battery(BATTERY_LEVEL_3);						//开机显示三格电量
	POWER->sys_state = SYS_ON;
	POWER->request_state = SYS_ON;
	POWER->send_state(SYS_ON);				
	TIMER->request_data_start();			//开机请求数据			
	TIMER->powerdown_start();				//30S无按键进入休眠状态
	
	LCD->student_id(STUDENT_ID);			//显示答题器配置序号，测试使用
	while(true)
	{
//		debug_fun();       					//调试用函数
		switch (POWER->sys_state)
        {
        	case SYS_ON:  
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLOCK->HFCLK_start();
					RADIO->wakeup();
					LCD->wakeup();							//LCD->wakeup函数在(唤醒)请求到数据后调用，否则若上一条指令是一键关机，会产生LCD亮一小会儿后立马灭的视觉
					POWER->request_state = SYS_SLEEP;
					POWER->send_state(SYS_SLEEP);			//发送唤醒请求	
					POWER->request_data_flg = false;	
					POWER->request_data_num = 0;			//请求数据计数重置为0
					TIMER->request_data_start();			//唤醒请求定时器
					TIMER->powerdown_start();				//30S无按键进入休眠状态
					TIMER->system_off_stop();				//待机超时自动关机定时器关闭
				}
				TIMER->event_handler();				
				RADIO->rx_data();				
				APP->command_handler();			
				APP->button_handler();
//				RTC->update();							//未处于答题状态才会显示RTC			
				ADC->update();
				LCD->update();	
				break;
        	case SYS_SLEEP: 
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLOCK->HFCLK_stop();			
					LCD->sleep();			
					RADIO->sleep();
					POWER->sys_off_count = 0;
					TIMER->system_off_start();		//待机超时自动关机定时器开启
				}
				TIMER->event_handler();
				APP->button_handler();
        		break;
        	default:
        		break;
        }
		__WFE();							//POWER DOWN，wait for EVENT	.
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










