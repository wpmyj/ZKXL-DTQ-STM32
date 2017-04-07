

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
	
	/* �ṹ�������ʼ�� */
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
	
	/* ϵͳ���������ܷ��͹ػ�״̬ */
	if((POWER->sys_state == SYS_ON) || (POWER->sys_state == SYS_SLEEP))
	{
//		send_system_state(SYS_OFF);	
//		nrf_delay_ms(15);
	}	
	GPIO_default();							//�ػ�ǰGPIO����Ϊ����̬����ֹ������������ϵͳ
	nrf_gpio_cfg_sense_input(KEY_PWR, NRF_GPIO_PIN_NOPULL, NRF_GPIO_PIN_SENSE_LOW);			
	NRF_POWER->SYSTEMOFF = 1;				//��������ģʽ
}


void POWER_On(void)
{
	if(0x00 == nrf_gpio_pin_read(KEY_PWR))			//������������Ϊ0		
	{
		TIMER_PowerOnStart();		//����ػ���Ϊ��ʱ����������
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
	/* ϵͳ״̬�л�ΪSLEEP */
	POWER->state_change_flg = true;	
	POWER->sys_state = SYS_SLEEP;
		
}

void POWER_Wakeup(void)
{
	/* ϵͳ״̬�л�ΪON */
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

















