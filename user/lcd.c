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
//临时的LCDRAM，用于保存需要临时显示的数据信息
uint8_t LCD_TmpRam[18] = {0x00,0x00,0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,0x00,0x00,
                         0x00,0x00,0x00,0x00,0x00,0x00};			
lcd_struct_t				lcd_struct;
										   
/*-------------------------------- Gocal Variable ----------------------------------------*/
lcd_struct_t				*LCD = &lcd_struct;
										   
/*-------------------------------- Function Declaration ----------------------------------*/


/*-------------------------------- Revision record ---------------------------------------*/




/*---------------------------------------------------------------------------------------*/



/*----------------------------------- 底层驱动 -------------------------------------------*/
	void LCD_write_data(uint8_t data, uint8_t count, uint8_t flag)
{
	uint8_t i;
	
	if(flag == 0)													//处理向左移位的操作
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
	else															//处理向右移位的操作
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
	//写命令的第九个时钟数据
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

//写多个数据
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

/*-------------------------------- 用户应用层函数---------------------------------------*/


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
		//IO口配置为高阻态，减功耗
		nrf_gpio_cfg_default(LCD_CSB); 		
		nrf_gpio_cfg_default(LCD_DATA);
		nrf_gpio_cfg_default(LCD_WRB);
	}
}
	


//系统长按开机前，用于提醒开机成功
	void LCD_diplay_on(void)
{
	uint8_t tmp_ram[18];				
	
	LCD_display_wakeup();
	memset(tmp_ram,0xFF,18);
	LCD_write_mult_data(0x00,18,tmp_ram); 
//	nrf_delay_ms(500);
}

//系统关机前调用，关闭LCD外设
	void LCD_diplay_off(void)
{
	LCD_write_command(LCDOFF);
	LCD_write_command(SYSDIS);  
	
	LCD_PowerOff();			//LCD电源控制关闭
	
	/* 所有控制管脚设置为输入高阻抗 */
//	nrf_gpio_cfg_output(LCD_CSB); 
//	nrf_gpio_cfg_output(LCD_DATA);
//	nrf_gpio_cfg_output(LCD_WRB);
//	nrf_gpio_pin_set(LCD_CSB);
//	nrf_gpio_pin_set(LCD_DATA);
//	nrf_gpio_pin_set(LCD_WRB);
}

//LCD进入休眠状态
	void LCD_display_sleep(void)
{
//	LCD_gpio_config(true);	
	
	/* 关掉刷新定时器 */
	TIMER_LcdUpdateStop();
	
	/* 清除LCD刷新flag */
	LCD->update_flg  = false;
	
	/* 关掉LCD显示 */
	LCD_write_command(LCDOFF);
	LCD_write_command(SYSDIS);  
	LCD_PowerOff();			//LCD电源控制关闭
	
	/* 所有控制管脚设置为输入高阻抗 */
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
	/* 设置所有控制管脚 */
	nrf_gpio_cfg_output(LCD_CSB); 
	nrf_gpio_cfg_output(LCD_DATA);
	nrf_gpio_cfg_output(LCD_WRB);
	nrf_gpio_cfg_output(LCD_POWER);
	nrf_gpio_pin_set(LCD_CSB);
	nrf_gpio_pin_set(LCD_DATA);
	nrf_gpio_pin_set(LCD_WRB);
	LCD_PowerOn();			//LCD电源控制打开
	nrf_delay_us(5);
	
	/* 配置LCD启动，刷新函数会自动刷新 */
	LCD_write_command(BIAS_1_3);	
	LCD_write_command(RC256);	
	LCD_write_command(SYSEN);	
	LCD_write_command(LCDON);
	
	/* 设置LCD刷新flag */
	LCD->update_flg  = true;
	
	/* 开启刷新定时器 */
	TIMER_LcdUpdateStart();
}

	void LCD_clear_screen(void)
{
	memset(LCD_RAM,0x00,18);
}

	void LCD_clear_letter(void)
{
		LCD_RAM[14] &= 0xF7;		//清除字母A
		LCD_RAM[12] &= 0xF7;        //清除字母B
		LCD_RAM[10] &= 0xF7;		//清除字母C
		LCD_RAM[8]  &= 0xF7;  		//清除字母D
		LCD_RAM[6]  &= 0xF7;        //显示字母E		
		LCD_RAM[4]  &= 0xF7;    	//清除字母F
}

	void LCD_diplay_letter(uint8_t data)
{
	if((data & A) == A)
		LCD_RAM[14] |= 0x08;        //显示字母A	
	else
		LCD_RAM[14] &= 0xF7;        //清除字母A

	if((data & B) == B)
		LCD_RAM[12] |= 0x08;        //显示字母B		
	else
		LCD_RAM[12] &= 0xF7;        //清除字母B

	if((data & C) == C)
		LCD_RAM[10] |= 0x08;        //显示字母C		
	else
		LCD_RAM[10] &= 0xF7;        //清除字母C

	if((data & D) == D)
		LCD_RAM[8] |= 0x08;        //显示字母D		
	else
		LCD_RAM[8] &= 0xF7;        //清除字母D

	if((data & E) == E)
		LCD_RAM[6] |= 0x08; 		//显示字母E		
	else
		LCD_RAM[6] &= 0xF7;         //清除字母E

	if((data & F) == F)
		LCD_RAM[4] |= 0x08;        //显示字母F		
	else
		LCD_RAM[4] &= 0xF7;        //清除字母F
}

	void LCD_display_true_false(uint8_t tmp)
{
	switch(tmp)
	{
		case JUDGE_CLEAR:
			LCD_RAM[0] &= 0xF7;//清除错
			LCD_RAM[2] &= 0xF7;//清除对
			break;
		case JUDGE_TRUE:
			LCD_RAM[0] &= 0xF7;//清除错
			LCD_RAM[2] |= 0x08;//显示对
			break;
		case JUDGE_FALSE:
			LCD_RAM[2] &= 0xF7;//清除对
			LCD_RAM[0] |= 0x08;//显示错
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
			LCD_RAM[14] &= 0xFD;//清除ok
			LCD_RAM[14] &= 0xFB;//清除传输错误
			break;
		case SEND_RESULT_OK:
			LCD_RAM[14] &= 0xFB;//清除传输错误
			LCD_RAM[14] |= 0x02;//显示ok
			break;
		case SEND_RESULT_FAIL:
			LCD_RAM[14] &= 0xFD;//清除ok
			LCD_RAM[14] |= 0x04;//显示传输错误
			break;
		default:
			break;
	}
}


	void LCD_diplay_signal(uint8_t tmp)					//显示信号强度
{	
	LCD_RAM[15] &= 0xF0;//清除所有信号显示
	
	LCD_RAM[16] |= 0x01;//显示信号标志
	
	switch(tmp)
	{
		case RSSI_0:
			LCD_RAM[16] |= 0x01;//显示信号标志
			break;
		case RSSI_1:
			LCD_RAM[15] |= 0x08;//一格信号
			break;
		case RSSI_2:
			LCD_RAM[15] |= 0x0C;//两格信号
			break;
		case RSSI_3:
			LCD_RAM[15] |= 0x0E;//三格信号
			break;
		case RSSI_4:
			LCD_RAM[15] |= 0x0F;//四格信号
			break;
		default:
			LCD_RAM[16] |= 0x01;//显示信号标志
			break;
	}	
}


	void LCD_diplay_battery_level(uint8_t tmp)			//显示电池电量
{
	
	LCD_RAM[17] &= 0xF7;	//清除所有电量显示
	LCD_RAM[16] &= 0xF1;	
	
	LCD_RAM[16] |= 0x02;			//显示电量框
	
	switch(tmp)
	{
		case BATTERY_LEVEL_0:
			break;
		case BATTERY_LEVEL_1:
			LCD_RAM[16] |= 0x04;	//一格电量
			break;
		case BATTERY_LEVEL_2:
			LCD_RAM[16] |= 0x0C;	//两格电量
			break;
		case BATTERY_LEVEL_3:
			LCD_RAM[16] |= 0x0C;	//三格电量
			LCD_RAM[17] |= 0x08;	//三格电量
			break;
		default:
			LCD_RAM[16] |= 0x0C;	//三格电量
			LCD_RAM[17] |= 0x08;	//三格电量
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
	LCD_RAM[14] |= 0x01;			//显示小数点
}

void LCD_clear_dot( void )
{
	LCD_RAM[14] &= 0xFE;			//清除小数点
}

//天喻面板数码管，从右到左，分别代表1~7
void LCD_diplay_segment(uint8_t num, uint8_t data)
{
	switch(num)
	{
		case 1:											//显示第一位数字
			modify_segment(0, 1, data);
		break;
		case 2:											//显示第二位数字
			modify_segment(2, 3, data);
		break;
		case 3:											//显示第三位数字
			modify_segment(4, 5, data);
		break;
		case 4:											//显示第四位数字
			modify_segment(6, 7, data);
		break;
		case 5:											//显示第五位数字
			modify_segment(8, 9, data);
		break;
		case 6:											//显示第六位数字
			modify_segment(10, 11, data);
		break;
		case 7:											//显示第六位数字
			modify_segment(12, 13, data);
		break;
		default:
			break;
	}
}


void LCD_display_student_id(uint16_t student_id)
{
	
	LCD_diplay_segment(7,(uint8_t)(student_id / 100) );			//百位
	LCD_diplay_segment(6,(uint8_t)((student_id / 10) % 10));	//十位
	LCD_diplay_segment(5,(uint8_t)(student_id % 10));		    //个位
}

void LCD_display_question_num(uint8_t question_num)
{
	LCD_diplay_segment(4,question_num / 10);			//十位
	LCD_diplay_segment(3,question_num % 10);		    //个位
}

void LCD_display_grade_value(uint8_t grade_value)
{
	LCD_diplay_segment(2,grade_value / 10);			//十位 ,评分和投票结果暂不可能大于9
	LCD_diplay_segment(1,grade_value % 10);		    //个位
}

//临时显示版本信息
void LCD_DisplayVerInfo(void)
{						
	LCD_display_student_id(VERSION_PROTOCOL);
	LCD_display_question_num(VERSION_LEVEL_1);
	LCD_display_grade_value(VERSION_LEVEL_2);
	LCD_write_mult_data(0x00,18,LCD_TmpRam);
}


/*---------------------------用户应用层-----------------------------------*/

	void LCD_display_update( void )
{	
	if(!RADIO->rx_window_on_flg)		//如果RX窗处于打开状态，不更新LCD显示，防止各个外设同时工作，把电池电压拉的很低
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
//				LCD_gpio_config(true);					//LCD配置IO口可用
				LCD_write_mult_data(0x00,18,LCD_RAM); 	//内存从0开始把LCD屏用到的都刷一遍
//				LCD_gpio_config(false);					//LCD引脚高阻态	
			}

			
		}
	}
}


void LCD_init(void)
{
	/* GPIO配置 */
	nrf_gpio_cfg_output(LCD_CSB);
	nrf_gpio_cfg_output(LCD_DATA);
	nrf_gpio_cfg_output(LCD_WRB);
	nrf_gpio_cfg_output(LCD_POWER);
	nrf_gpio_pin_set(LCD_CSB);
	nrf_gpio_pin_set(LCD_DATA);
	nrf_gpio_pin_set(LCD_WRB);
	LCD_PowerOff();			//LCD电源控制关闭
	nrf_delay_us(10);
	
	/* 写命令 */
//	LCD_write_command(BIAS_1_3);	
//	LCD_write_command(RC256);	
//	LCD_write_command(SYSEN);	
//	LCD_write_mult_data(0x00,18,LCD_RAM);
//	LCD_write_command(LCDON);
	
	/* 结构体参数初始化 */
	LCD->update_flg   = true;
	
}

#define DISPLAY_TIME 		(300)						//显示的时间
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




