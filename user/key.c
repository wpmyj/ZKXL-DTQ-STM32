

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
	
	//全局变量赋初值
	KEY->press_flg = false;
	KEY->run_flg =false;
	KEY->state = KEY_SCAN;
	KEY->value = 0x00;
}


void button_event_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
	uint16_t status;
	
	if(12 == pin)	//RF刷卡完成，触发中断引脚
	{
		status = M24SR_KillSession();			//关掉RF，打开I2C
		status = TT4_ReadNDEF(NFC.DataRead);
		status = M24SR_Deselect();		
		PARAMETER_update();						//更新参数
		LCD_display_student_id(STUDENT_ID);			//重新显示学号
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
	
//	//NFC中断脚
//	nrf_gpio_cfg_input(12, NRF_GPIO_PIN_NOPULL);
//	err_code = nrf_drv_gpiote_in_init(12, &in_config, button_event_handler);
//	APP_ERROR_CHECK(err_code);
//	nrf_drv_gpiote_in_event_enable(12, false);
//	
//	//rtc中断脚
//	nrf_gpio_cfg_input(RTC_I2C_INT, NRF_GPIO_PIN_NOPULL);
//	err_code = nrf_drv_gpiote_in_init(RTC_I2C_INT, &in_config, button_event_handler);
//	APP_ERROR_CHECK(err_code);
//	nrf_drv_gpiote_in_event_enable(RTC_I2C_INT, false);
}

void KEY_Scan(void)
{
	static uint8_t key_value = 0;
	static uint8_t value = 0;
	static uint16_t key_power_count = 0;		//记录power键按下时间
	static uint16_t KeyFnCount = 0;			//记录Fn键按下时间
	uint8_t key_col_value = 0;
	uint8_t key_row_value = 0;
	
	
	if(true == KEY->run_flg)
	{
		KEY->run_flg = false; //运行标志清零
		
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

				KEY->state = KEY_ONE;					//更新按键状态
			    TIMER_ButtonStart();					//启动定时器
			    
			
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
				
				if(value == key_value)		//如果两个值相等，消抖完成,有效按键
			    {
					KEY->state = KEY_TWO;	
				}
				else						//无效按键
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
					/* 如果睡眠，先唤醒 */
					if(SYS_SLEEP == POWER->sys_state)
					{
						POWER_Wakeup();
					}
					value = 0x1E;
					key_power_count++;
					if(40 == key_power_count)		//长按power键3S = 50ms * 60次
					{
						POWER_Off();
					}
				}
				
				if(0x8B == value)
				{
					KeyFnCount++;
					if(KeyFnCount > 100)		//长按Fn大于5秒
					{
						APP->DisplayVerFlg = true;
					}
				}
				
				if(value != key_value)		//如果两个值不相等，说明按键已经释放，则认为按键OK，并停止定时器,
			    {
					/* 如果系统睡眠，按任意键直接唤醒 */
					if(SYS_SLEEP == POWER->sys_state)
					{
						POWER_Wakeup();
						TIMER_ButtonStop();
						KEY->state = KEY_SCAN;
						key_value = 0;
						return;						//直接返回，唤醒键不作为按键触发标志
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
							KEY->value = 15;    // 上题
							break;
						case 0x17:
							KEY->value = 16;    // 下题
							break;
						case 0x8B:
							KEY->value = 12;    // Fn
							break;
						case 0x8E:
							KEY->value = 14;    // OK
							break;
						case 0x4E:
							KEY->value = 13;    // 删除
							break;
						case 0x1E:
							KEY->value = 11;   //电源键				
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












