

/*-------------------------------- Include File -----------------------------------------*/
#include "key.h"

/*-------------------------------- Macro definition -------------------------------------*/

/*-------------------------------- Local Variable ----------------------------------------*/
key_struct_t				key_struct;

/*-------------------------------- Gocal Variable ----------------------------------------*/
key_struct_t				*KEY = &key_struct;

/*-------------------------------- Function Declaration ----------------------------------*/

/*-------------------------------- Revision record ---------------------------------------*/

void KEY_init(void)
{
	nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
	nrf_gpio_cfg_input(KEY_PWR, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_range_cfg_input(KEY_ROW1, KEY_ROW4, NRF_GPIO_PIN_NOPULL);
	
	key_button_init();
	
	//ȫ�ֱ�������ֵ
	KEY->press_flg = false;
	KEY->run_flg =false;
	KEY->state = KEY_SCAN;
	KEY->value = 0x00;
}


void button_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	uint16_t status;
	
	if(12 == pin)	//RFˢ����ɣ������ж�����
	{
		status = M24SR_KillSession();			//�ص�RF����I2C
		status = TT4_ReadNDEF(NFC.DataRead);
		status = M24SR_Deselect();		
		PARAMETER_update();						//���²���
		LCD_display_student_id(STUDENT_ID);			//������ʾѧ��
	}
	else
	{
		KEY->run_flg = true;
	}
}

 void key_button_init(void)
{
	uint8_t i;
	ret_code_t err_code;
	
	err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
	
	nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(false);
	
	for(i = KEY_ROW1;i < 4;i++)
	{
		err_code = nrf_drv_gpiote_in_init(i, &in_config, button_event_handler);
		APP_ERROR_CHECK(err_code);
		nrf_drv_gpiote_in_event_enable(i,false);
	}

	err_code = nrf_drv_gpiote_in_init(30, &in_config, button_event_handler);
	APP_ERROR_CHECK(err_code);
	nrf_drv_gpiote_in_event_enable(30, false);
	
//	//NFC�жϽ�
//	nrf_gpio_cfg_input(12, NRF_GPIO_PIN_NOPULL);
//	err_code = nrf_drv_gpiote_in_init(12, &in_config, button_event_handler);
//	APP_ERROR_CHECK(err_code);
//	nrf_drv_gpiote_in_event_enable(12, false);
//	
//	//rtc�жϽ�
//	nrf_gpio_cfg_input(RTC_I2C_INT, NRF_GPIO_PIN_NOPULL);
//	err_code = nrf_drv_gpiote_in_init(RTC_I2C_INT, &in_config, button_event_handler);
//	APP_ERROR_CHECK(err_code);
//	nrf_drv_gpiote_in_event_enable(RTC_I2C_INT, false);
}

void KEY_Scan(void)
{
	static uint8_t key_value = 0;
	static uint8_t value = 0;
	static uint16_t key_power_count = 0;		//��¼power������ʱ��
	static uint16_t KeyFnCount = 0;			//��¼Fn������ʱ��
	uint8_t key_col_value = 0;
	uint8_t key_row_value = 0;
	
	
	if(true == KEY->run_flg)
	{
		KEY->run_flg = false; //���б�־����
		
		switch(KEY->state)
		{
			case KEY_SCAN:
				key_row_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_input(KEY_COL1, KEY_COL4, NRF_GPIO_PIN_NOPULL);
				key_col_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
				key_value = (key_row_value & 0x0F) | (key_col_value & 0xF0);
				
			    if(0x00 == nrf_gpio_pin_read(KEY_PWR))
				{
					key_value = 0x1E;
					key_power_count = 0;
				}
				
				if(0x8B == key_value)
				{
					KeyFnCount = 0;
				}

				KEY->state = KEY_ONE;					//���°���״̬
			    TIMER_ButtonStart();					//������ʱ��
			    
			
			break;
			case KEY_ONE:
				key_row_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_input(KEY_COL1, KEY_COL4, NRF_GPIO_PIN_NOPULL);
				key_col_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
				value = (key_row_value & 0x0F) | (key_col_value & 0xF0);
				
			    if(0x00 == nrf_gpio_pin_read(KEY_PWR))
				{
					value = 0x1E;
					key_power_count++;
				}
				
				if(0x8B == value)
				{
					KeyFnCount++;
				}				
				
				if(value == key_value)		//�������ֵ��ȣ��������,��Ч����
			    {
					KEY->state = KEY_TWO;	
				}
				else						//��Ч����
				{
					TIMER_ButtonStop();	
					KEY->state = KEY_SCAN;
					key_value = 0;
				}

			break;
			case KEY_TWO:
				key_row_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_input(KEY_COL1, KEY_COL4, NRF_GPIO_PIN_NOPULL);
				key_col_value = nrf_gpio_port_read(KEY_PORT);
				nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
				value = (key_row_value & 0x0F) | (key_col_value & 0xF0);
				
			    if(0x00 == nrf_gpio_pin_read(KEY_PWR))
				{
					/* ���˯�ߣ��Ȼ��� */
					if(SYS_SLEEP == POWER->sys_state)
					{
						POWER_Wakeup();
					}
					value = 0x1E;
					key_power_count++;
					if(40 == key_power_count)		//����power��3S = 50ms * 60��
					{
						POWER_Off();
					}
				}
				
				if(0x8B == value)
				{
					KeyFnCount++;
					if(KeyFnCount > 100)		//����Fn����5��
					{
						APP->DisplayVerFlg = true;
					}
				}
				
				if(value != key_value)		//�������ֵ����ȣ�˵�������Ѿ��ͷţ�����Ϊ����OK����ֹͣ��ʱ��,
			    {
					/* ���ϵͳ˯�ߣ��������ֱ�ӻ��� */
					if(SYS_SLEEP == POWER->sys_state)
					{
						POWER_Wakeup();
						TIMER_ButtonStop();
						KEY->state = KEY_SCAN;
						key_value = 0;
						return;						//ֱ�ӷ��أ����Ѽ�����Ϊ����������־
					}
					
					switch(key_value)
					{
						case 0x4D:
							KEY->value = 1;   //
							break;
						case 0x2D:
							KEY->value = 2;   //
							break;
						case 0x1D:
							KEY->value = 3;   //
							break;
						case 0x4B:
							KEY->value = 4;	  //
							break;
						case 0x2B:
							KEY->value = 5;	  //
							break;
						case 0x1B:
							KEY->value = 6;    //
							break;
						case 0x87:
							KEY->value = 7;    //
							break;
						case 0x47:
							KEY->value = 8;    //
							break;
						case 0x27:
							KEY->value = 9;    //
							break;
						case 0x8D:
							KEY->value = 15;    // ����
							break;
						case 0x17:
							KEY->value = 16;    // ����
							break;
						case 0x8B:
							KEY->value = 12;    // Fn
							break;
						case 0x8E:
							KEY->value = 14;    // OK
							break;
						case 0x4E:
							KEY->value = 13;    // ɾ��
							break;
						case 0x1E:
							KEY->value = 11;   //��Դ��				
						default:
							KEY->value = 0;
							break;
					}
					KEY->press_flg = true;	
					TIMER_ButtonStop();
					KEY->state = KEY_SCAN;
					key_value = 0;
				}
			break;
				
				
			default: 
				KEY->state = KEY_SCAN;
				KEY->value = 0;
				key_value = 0;
	
		}
//		key_button_init();
	}
}












