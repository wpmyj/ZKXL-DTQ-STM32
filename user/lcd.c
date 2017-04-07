/** @file lcd.c
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
/*-------------------------------- Include File -----------------------------------------*/
#include "lcd.h"
/*-------------------------------- Macro definition -------------------------------------*/
//#define	ENABLE_LCD_DEBUGLOG
#ifdef	ENABLE_LCD_DEBUGLOG
	#define	LCD_DebugLog			app_trace_log
#else
	#define	LCD_DebugLog(...)
#endif //ENABLE_LCD_DEBUGLOG
/*-------------------------------- Local Variable ----------------------------------------*/

uint8_t 					LCD_RAM[18] = {0x00,0x00,0x00,0x00,0x00,0x00,
                                           0x00,0x00,0x00,0x00,0x00,0x00,
                                           0x00,0x00,0x00,0x00,0x00,0x00};								
//��ʱ��LCDRAM�����ڱ�����Ҫ��ʱ��ʾ��������Ϣ
uint8_t LCD_TmpRam[18] = {0x00,0x00,0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,0x00,0x00};			
lcd_struct_t				lcd_struct;
										   
/*-------------------------------- Gocal Variable ----------------------------------------*/
lcd_struct_t				*LCD = &lcd_struct;
										   
/*-------------------------------- Function Declaration ----------------------------------*/


/*-------------------------------- Revision record ---------------------------------------*/




/*---------------------------------------------------------------------------------------*/



/*----------------------------------- �ײ����� -------------------------------------------*/
	void LCD_write_data(uint8_t data, uint8_t count, uint8_t flag)
{
	uint8_t i;
	
	if(flag == 0)													//����������λ�Ĳ���
	{
		for (i=0; i<count; i++)
		{
			nrf_gpio_pin_clear(LCD_WRB);
			LCD_DRIVE_DELAY();
			nrf_gpio_pin_write(LCD_DATA, data & 0x80);
			LCD_DRIVE_DELAY();
			nrf_gpio_pin_set(LCD_WRB);
			LCD_DRIVE_DELAY();
			data <<= 1;
		}
	}
	else															//����������λ�Ĳ���
	{
		for (i=0; i<count; i++)
		{
			nrf_gpio_pin_clear(LCD_WRB);
			LCD_DRIVE_DELAY();
			nrf_gpio_pin_write(LCD_DATA, data & 0x01);
			LCD_DRIVE_DELAY();
			nrf_gpio_pin_set(LCD_WRB);
			LCD_DRIVE_DELAY();
			data >>= 1;
		}
	}
}

	void LCD_write_command(uint8_t cmd)
{
	nrf_gpio_pin_clear(LCD_CSB);
	LCD_DRIVE_DELAY();
	LCD_write_data(0x80, 3, 0);
	LCD_write_data(cmd, 8, 0);
	//д����ĵھŸ�ʱ������
	nrf_gpio_pin_clear(LCD_WRB);
	LCD_DRIVE_DELAY();
	nrf_gpio_pin_clear(LCD_DATA);
	LCD_DRIVE_DELAY();
	nrf_gpio_pin_set(LCD_WRB);
	LCD_DRIVE_DELAY();
	nrf_gpio_pin_set(LCD_CSB);
	LCD_DRIVE_DELAY();
}


	void LCD_write_signle_data(uint8_t addr, uint8_t data)
{
	nrf_gpio_pin_clear(LCD_CSB);
	LCD_DRIVE_DELAY();
	LCD_write_data(0xa0, 3, 0);
	LCD_write_data(addr << 2, 6, 0);
	LCD_write_data(data, 4, 1);
	nrf_gpio_pin_set(LCD_CSB);
	LCD_DRIVE_DELAY();
}

//д�������
	void LCD_write_mult_data(uint8_t addr, uint8_t len, uint8_t * data)
{
	uint8_t i;
	
	nrf_gpio_pin_clear(LCD_CSB);
	LCD_DRIVE_DELAY();
	LCD_write_data(0xa0, 3, 0);
	LCD_write_data(addr << 2, 6, 0);
	for(i=0;i<len;i++)
	{
	    LCD_write_data(data[i], 4, 1);
	}
	nrf_gpio_pin_set(LCD_CSB);
	LCD_DRIVE_DELAY();
}

	void clear_ram(uint8_t ram1, uint8_t ram2)
{
	if(APP->DisplayVerFlg)
	{
		LCD_TmpRam[ram1] &= 0x08;
		LCD_TmpRam[ram2] &= 0x00;
	}
	else
	{
		LCD_RAM[ram1] &= 0x08;
		LCD_RAM[ram2] &= 0x00;
	}

}

	void modify_ram(uint8_t ram1, uint8_t ram2, uint8_t data1, uint8_t data2)
{
	if(APP->DisplayVerFlg)
	{
		LCD_TmpRam[ram1] |= data1;
		LCD_TmpRam[ram2] |= data2;
	}
	else
	{
		LCD_RAM[ram1] |= data1;
		LCD_RAM[ram2] |= data2;
	}

}

	void modify_segment(uint8_t ram1, uint8_t ram2, uint8_t data)
{
	switch(data)
	{
		case 0:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x5, 0xF);
			break;
		case 1:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x5, 0x0);		
			break;
		case 2:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x3, 0xD);		
		break;
		case 3:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x7, 0x9);
		break;
		case 4:
			clear_ram(ram1, ram2);	
			modify_ram(ram1, ram2, 0x7, 0x2);	
		break;
		case 5:
			clear_ram(ram1, ram2);	
			modify_ram(ram1, ram2, 0x6, 0xB);		
		break;
		case 6:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x6, 0xF);		
		break;
		case 7:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x5, 0x1);		
		break;
		case 8:
			clear_ram(ram1, ram2);		
			modify_ram(ram1, ram2, 0x7, 0xF);		
		break;
		case 9:
			clear_ram(ram1, ram2);
			modify_ram(ram1, ram2, 0x7, 0xB);		
		break;
		case 10:
			clear_ram(ram1, ram2);	
		break;
		default:
			break;
	}
}

/*-------------------------------- �û�Ӧ�ò㺯��---------------------------------------*/


 void LCD_gpio_config(bool tmp)
{
	if(tmp)
	{
		nrf_gpio_cfg_output(LCD_CSB); 		
		nrf_gpio_cfg_output(LCD_DATA);
		nrf_gpio_cfg_output(LCD_WRB);
	}
	else
	{
		//IO������Ϊ����̬��������
		nrf_gpio_cfg_default(LCD_CSB); 		
		nrf_gpio_cfg_default(LCD_DATA);
		nrf_gpio_cfg_default(LCD_WRB);
	}
}
	


//ϵͳ��������ǰ���������ѿ����ɹ�
	void LCD_diplay_on(void)
{
	uint8_t tmp_ram[18];				
	
	LCD_display_wakeup();
	memset(tmp_ram,0xFF,18);
	LCD_write_mult_data(0x00,18,tmp_ram); 
//	nrf_delay_ms(500);
}

//ϵͳ�ػ�ǰ���ã��ر�LCD����
	void LCD_diplay_off(void)
{
	LCD_write_command(LCDOFF);
	LCD_write_command(SYSDIS);  
	
	LCD_PowerOff();			//LCD��Դ���ƹر�
	
	/* ���п��ƹܽ�����Ϊ������迹 */
//	nrf_gpio_cfg_output(LCD_CSB); 
//	nrf_gpio_cfg_output(LCD_DATA);
//	nrf_gpio_cfg_output(LCD_WRB);
//	nrf_gpio_pin_set(LCD_CSB);
//	nrf_gpio_pin_set(LCD_DATA);
//	nrf_gpio_pin_set(LCD_WRB);
}

//LCD��������״̬
	void LCD_display_sleep(void)
{
//	LCD_gpio_config(true);	
	
	/* �ص�ˢ�¶�ʱ�� */
	TIMER_LcdUpdateStop();
	
	/* ���LCDˢ��flag */
	LCD->update_flg  = false;
	
	/* �ص�LCD��ʾ */
	LCD_write_command(LCDOFF);
	LCD_write_command(SYSDIS);  
	LCD_PowerOff();			//LCD��Դ���ƹر�
	
	/* ���п��ƹܽ�����Ϊ������迹 */
//	LCD_gpio_config(false);	
//	nrf_gpio_cfg_output(LCD_CSB); 
//	nrf_gpio_cfg_output(LCD_DATA);
//	nrf_gpio_cfg_output(LCD_WRB);
//	nrf_gpio_pin_set(LCD_CSB);
//	nrf_gpio_pin_set(LCD_DATA);
//	nrf_gpio_pin_set(LCD_WRB);
}

	void LCD_display_wakeup(void)
{
	/* �������п��ƹܽ� */
	nrf_gpio_cfg_output(LCD_CSB); 
	nrf_gpio_cfg_output(LCD_DATA);
	nrf_gpio_cfg_output(LCD_WRB);
	nrf_gpio_cfg_output(LCD_POWER);
	nrf_gpio_pin_set(LCD_CSB);
	nrf_gpio_pin_set(LCD_DATA);
	nrf_gpio_pin_set(LCD_WRB);
	LCD_PowerOn();			//LCD��Դ���ƴ�
	nrf_delay_us(5);
	
	/* ����LCD������ˢ�º������Զ�ˢ�� */
	LCD_write_command(BIAS_1_3);	
	LCD_write_command(RC256);	
	LCD_write_command(SYSEN);	
	LCD_write_command(LCDON);
	
	/* ����LCDˢ��flag */
	LCD->update_flg  = true;
	
	/* ����ˢ�¶�ʱ�� */
	TIMER_LcdUpdateStart();
}

	void LCD_clear_screen(void)
{
	memset(LCD_RAM,0x00,18);
}

	void LCD_clear_letter(void)
{
		LCD_RAM[14] &= 0xF7;		//�����ĸA
		LCD_RAM[12] &= 0xF7;        //�����ĸB
		LCD_RAM[10] &= 0xF7;		//�����ĸC
		LCD_RAM[8]  &= 0xF7;  		//�����ĸD
		LCD_RAM[6]  &= 0xF7;        //��ʾ��ĸE		
		LCD_RAM[4]  &= 0xF7;    	//�����ĸF
}

	void LCD_diplay_letter(uint8_t data)
{
	if((data & A) == A)
		LCD_RAM[14] |= 0x08;        //��ʾ��ĸA	
	else
		LCD_RAM[14] &= 0xF7;        //�����ĸA

	if((data & B) == B)
		LCD_RAM[12] |= 0x08;        //��ʾ��ĸB		
	else
		LCD_RAM[12] &= 0xF7;        //�����ĸB

	if((data & C) == C)
		LCD_RAM[10] |= 0x08;        //��ʾ��ĸC		
	else
		LCD_RAM[10] &= 0xF7;        //�����ĸC

	if((data & D) == D)
		LCD_RAM[8] |= 0x08;        //��ʾ��ĸD		
	else
		LCD_RAM[8] &= 0xF7;        //�����ĸD

	if((data & E) == E)
		LCD_RAM[6] |= 0x08; 		//��ʾ��ĸE		
	else
		LCD_RAM[6] &= 0xF7;         //�����ĸE

	if((data & F) == F)
		LCD_RAM[4] |= 0x08;        //��ʾ��ĸF		
	else
		LCD_RAM[4] &= 0xF7;        //�����ĸF
}

	void LCD_display_true_false(uint8_t tmp)
{
	switch(tmp)
	{
		case JUDGE_CLEAR:
			LCD_RAM[0] &= 0xF7;//�����
			LCD_RAM[2] &= 0xF7;//�����
			break;
		case JUDGE_TRUE:
			LCD_RAM[0] &= 0xF7;//�����
			LCD_RAM[2] |= 0x08;//��ʾ��
			break;
		case JUDGE_FALSE:
			LCD_RAM[2] &= 0xF7;//�����
			LCD_RAM[0] |= 0x08;//��ʾ��
			break;
		default:
			break;
	}
}

	void LCD_display_ok_failure(uint8_t tmp)
{
	switch(tmp)
	{
		case SEND_RESULT_CLEAR:
			LCD_RAM[14] &= 0xFD;//���ok
			LCD_RAM[14] &= 0xFB;//����������
			break;
		case SEND_RESULT_OK:
			LCD_RAM[14] &= 0xFB;//����������
			LCD_RAM[14] |= 0x02;//��ʾok
			break;
		case SEND_RESULT_FAIL:
			LCD_RAM[14] &= 0xFD;//���ok
			LCD_RAM[14] |= 0x04;//��ʾ�������
			break;
		default:
			break;
	}
}


	void LCD_diplay_signal(uint8_t tmp)					//��ʾ�ź�ǿ��
{	
	LCD_RAM[15] &= 0xF0;//��������ź���ʾ
	
	LCD_RAM[16] |= 0x01;//��ʾ�źű�־
	
	switch(tmp)
	{
		case RSSI_0:
			LCD_RAM[16] |= 0x01;//��ʾ�źű�־
			break;
		case RSSI_1:
			LCD_RAM[15] |= 0x08;//һ���ź�
			break;
		case RSSI_2:
			LCD_RAM[15] |= 0x0C;//�����ź�
			break;
		case RSSI_3:
			LCD_RAM[15] |= 0x0E;//�����ź�
			break;
		case RSSI_4:
			LCD_RAM[15] |= 0x0F;//�ĸ��ź�
			break;
		default:
			LCD_RAM[16] |= 0x01;//��ʾ�źű�־
			break;
	}	
}


	void LCD_diplay_battery_level(uint8_t tmp)			//��ʾ��ص���
{
	
	LCD_RAM[17] &= 0xF7;	//������е�����ʾ
	LCD_RAM[16] &= 0xF1;	
	
	LCD_RAM[16] |= 0x02;			//��ʾ������
	
	switch(tmp)
	{
		case BATTERY_LEVEL_0:
			break;
		case BATTERY_LEVEL_1:
			LCD_RAM[16] |= 0x04;	//һ�����
			break;
		case BATTERY_LEVEL_2:
			LCD_RAM[16] |= 0x0C;	//�������
			break;
		case BATTERY_LEVEL_3:
			LCD_RAM[16] |= 0x0C;	//�������
			LCD_RAM[17] |= 0x08;	//�������
			break;
		default:
			LCD_RAM[16] |= 0x0C;	//�������
			LCD_RAM[17] |= 0x08;	//�������
			break;
	}	
}


void LCD_clear_segment(void)
{
	modify_segment(0, 1, 10);
	modify_segment(2, 3, 10);
	modify_segment(4, 5, 10);
	modify_segment(6, 7, 10);
	modify_segment(8, 9, 10);
	modify_segment(10,11, 10);
	modify_segment(12,13, 10);
}

void LCD_diplay_dot( void )
{
	LCD_RAM[14] |= 0x01;			//��ʾС����
}

void LCD_clear_dot( void )
{
	LCD_RAM[14] &= 0xFE;			//���С����
}

//�����������ܣ����ҵ��󣬷ֱ����1~7
void LCD_diplay_segment(uint8_t num, uint8_t data)
{
	switch(num)
	{
		case 1:											//��ʾ��һλ����
			modify_segment(0, 1, data);
		break;
		case 2:											//��ʾ�ڶ�λ����
			modify_segment(2, 3, data);
		break;
		case 3:											//��ʾ����λ����
			modify_segment(4, 5, data);
		break;
		case 4:											//��ʾ����λ����
			modify_segment(6, 7, data);
		break;
		case 5:											//��ʾ����λ����
			modify_segment(8, 9, data);
		break;
		case 6:											//��ʾ����λ����
			modify_segment(10, 11, data);
		break;
		case 7:											//��ʾ����λ����
			modify_segment(12, 13, data);
		break;
		default:
			break;
	}
}


void LCD_display_student_id(uint16_t student_id)
{
	
	LCD_diplay_segment(7,(uint8_t)(student_id / 100) );			//��λ
	LCD_diplay_segment(6,(uint8_t)((student_id / 10) % 10));	//ʮλ
	LCD_diplay_segment(5,(uint8_t)(student_id % 10));		    //��λ
}

void LCD_display_question_num(uint8_t question_num)
{
	LCD_diplay_segment(4,question_num / 10);			//ʮλ
	LCD_diplay_segment(3,question_num % 10);		    //��λ
}

void LCD_display_grade_value(uint8_t grade_value)
{
	LCD_diplay_segment(2,grade_value / 10);			//ʮλ ,���ֺ�ͶƱ����ݲ����ܴ���9
	LCD_diplay_segment(1,grade_value % 10);		    //��λ
}

//��ʱ��ʾ�汾��Ϣ
void LCD_DisplayVerInfo(void)
{						
	LCD_display_student_id(VERSION_PROTOCOL);
	LCD_display_question_num(VERSION_LEVEL_1);
	LCD_display_grade_value(VERSION_LEVEL_2);
	LCD_write_mult_data(0x00,18,LCD_TmpRam);
}


/*---------------------------�û�Ӧ�ò�-----------------------------------*/

	void LCD_display_update( void )
{	
	if(!RADIO->rx_window_on_flg)		//���RX�����ڴ�״̬��������LCD��ʾ����ֹ��������ͬʱ�������ѵ�ص�ѹ���ĺܵ�
	{
		if(LCD->update_flg)
		{	
			LCD->update_flg  = false;
			if(APP->DisplayVerFlg)
			{
				LCD_DisplayVerInfo();
				APP->DisplayVerFlg = false;
			}
			else
			{
//				LCD_gpio_config(true);					//LCD����IO�ڿ���
				LCD_write_mult_data(0x00,18,LCD_RAM); 	//�ڴ��0��ʼ��LCD���õ��Ķ�ˢһ��
//				LCD_gpio_config(false);					//LCD���Ÿ���̬	
			}

			
		}
	}
}


void LCD_init(void)
{
	/* GPIO���� */
	nrf_gpio_cfg_output(LCD_CSB);
	nrf_gpio_cfg_output(LCD_DATA);
	nrf_gpio_cfg_output(LCD_WRB);
	nrf_gpio_cfg_output(LCD_POWER);
	nrf_gpio_pin_set(LCD_CSB);
	nrf_gpio_pin_set(LCD_DATA);
	nrf_gpio_pin_set(LCD_WRB);
	LCD_PowerOff();			//LCD��Դ���ƹر�
	nrf_delay_us(10);
	
	/* д���� */
//	LCD_write_command(BIAS_1_3);	
//	LCD_write_command(RC256);	
//	LCD_write_command(SYSEN);	
//	LCD_write_mult_data(0x00,18,LCD_RAM);
//	LCD_write_command(LCDON);
	
	/* �ṹ�������ʼ�� */
	LCD->update_flg   = true;
	
}

#define DISPLAY_TIME 		(300)						//��ʾ��ʱ��
void LCD_diplay_test(void)
{
//		LCD->display_send_result(SEND_RESULT_OK);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_send_result(SEND_RESULT_FAIL);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
		
//		LCD->display_battery(BATTERY_LEVEL_0);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_battery(BATTERY_LEVEL_1);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_battery(BATTERY_LEVEL_2);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_battery(BATTERY_LEVEL_3);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
		
//		LCD->display_judge(JUDGE_CLEAR);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_judge(JUDGE_TRUE);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_judge(JUDGE_FALSE);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
		
//		LCD->display_signal(RSSI_0);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_signal(RSSI_1);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_signal(RSSI_2);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_signal(RSSI_3);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
//		LCD->display_signal(RSSI_4);
//		LCD_write_mult_data(0x00,18,LCD_RAM);
//		nrf_delay_ms(500);
}

/** @} */
/** @} */




