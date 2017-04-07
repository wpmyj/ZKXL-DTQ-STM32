/** @file key_scan.c
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V4.73.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.05.19
 * $Revision: 	V1000
 *
 */

#include "key_scan.h"

#define KEY_DEBUG

#ifdef KEY_DEBUG
#define key_debug  debug_printf   
#else  
#define key_debug(...)                    
#endif 




void Key_Init(void)
{
	nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
	nrf_gpio_cfg_input(KEY_PWR, NRF_GPIO_PIN_NOPULL);
	nrf_gpio_range_cfg_input(KEY_ROW1, KEY_ROW4, NRF_GPIO_PIN_NOPULL);
}

static bool Key_down(void)
{
	nrf_gpio_port_write(KEY_PORT, 0x00 | KEY_VALUE_MASK);		//����λ���0������λ���ֲ���
	
	if(0x0F != nrf_gpio_port_read(KEY_PORT) || 0x00 == nrf_gpio_pin_read(KEY_PWR))
	{
		nrf_delay_ms(15);
		if(0x0F != nrf_gpio_port_read(KEY_PORT) || 0x00 == nrf_gpio_pin_read(KEY_PWR))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

uint8_t Key_scan(bool key_down_wait_flag)
{
	uint8_t key_value = 0;
	uint8_t key_col_value = 0;
	uint8_t key_row_value = 0;
	uint8_t value = 0xFF;
	uint16_t key_hold_time = 0;
		
	if(Key_down())
	{
//		system_timeout_stop();									//�а����������¿�ʼ��ʱ��ʱ��
//		system_timeout_start();
		key_row_value = nrf_gpio_port_read(KEY_PORT);
		nrf_gpio_range_cfg_input(KEY_COL1, KEY_COL4, NRF_GPIO_PIN_NOPULL);
		key_col_value = nrf_gpio_port_read(KEY_PORT);
		nrf_gpio_range_cfg_output(KEY_COL1, KEY_COL4);
		
		key_value = (key_row_value & 0x0F) | (key_col_value & 0xF0);
		if(0x00 == nrf_gpio_pin_read(KEY_PWR))
		{
			key_value = 0x1E;
		}
		
		
		switch(key_value)
		{
			case 0x4D:
				value = 1;   //
				break;
			case 0x2D:
				value = 2;   //
				break;
			case 0x1D:
				value = 3;   //
				break;
			case 0x4B:
				value = 4;	  //
				break;
			case 0x2B:
				value = 5;	  //
				break;
			case 0x1B:
				value = 6;    //
				break;
			case 0x87:
				value = 7;    //
				break;
			case 0x47:
				value = 8;    //
				break;
			case 0x27:
				value = 9;    //
				break;
			case 0x8D:
				value = 15;    // ����
				break;
			case 0x17:
				value = 16;    // ����
				break;
			case 0x8B:
				value = 12;    // Fn
				break;
			case 0x8E:
				value = 14;    // OK
				break;
			case 0x4E:
				value = 13;    // ɾ��
				break;
			case 0x1E:
				value = 11;   //��Դ��
//				break;	
//			case 0x17:
//				value = 10;   //
//				break;					
			default:
				break;
		}
		
		
		if(key_down_wait_flag)										//����򿪰������µȴ���־������Ҫ�ȴ��������º���з�Ӧ
		{	
			while(Key_down())
			{	
				key_hold_time++;
				if((key_hold_time >= 150)&&(value == 11))
				{
//					LCD_diplay_off();								//����3s�ر���Ļ
					while(Key_down());								//�ȴ������ͷ�
//					power_manage();									//�ͷź�ػ�
				}
			}
		}
		key_debug("[Key]: key value: %d\r\n", value);	//������µİ�����ֵ
	
	}
	return value;
}





/** @} */
/** @} */
