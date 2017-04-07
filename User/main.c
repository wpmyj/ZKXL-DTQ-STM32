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
	
	POWER_On();								//������������
	
	UART_init();							//UART����ʼ���ܽ�����
	debug_logic_init();	
	NFC_Init();								
	RTC_Init();
	ADC_init();
	RADIO_init();
	PARAMETER_init();
	APP_init();								// APP_initҪ����NFC��,��ΪNFC��ȡUIDҪ��APP_init��ʹ�� 		

	TIMER_RxWindowStart();
	TIMER_LcdUpdateStart();
	POWER->sys_state = SYS_ON;
	POWER->request_state = SYS_ON;
	TIMER_SysSleepStart();				
	
	LCD_display_student_id(STUDENT_ID);			
	while(true)
	{
//		debug_fun();       					//�����ú���
		switch (POWER->sys_state)
        {
        	case SYS_ON:  
				if(POWER->state_change_flg)
				{
					POWER->state_change_flg = false;
					CLK_HFCLKStart();
					RADIO_Wakeup();
//					LCD_display_wakeup();				//LCD���ѷ���POWER���Ѻ������������Ӿ���ʱ			
					POWER->request_state = SYS_SLEEP;	
					POWER->request_data_flg = false;	
					TIMER_SysSleepStart();				//30S�ް�����������״̬
					TIMER_SysOffStop();					//45min�ް����Զ��ػ�
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
					TIMER_SysOffStart();		//������ʱ�Զ��ػ���ʱ������
				}
				TIMER_MyEventHandler();
				APP_KeyHandler();
        		break;
        	default:
        		break;
        }
		__WFE();							//POWER DOWN��wait for EVENT	
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










