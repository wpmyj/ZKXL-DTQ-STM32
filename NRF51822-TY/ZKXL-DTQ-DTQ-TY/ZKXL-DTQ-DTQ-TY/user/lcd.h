/** @file lcd.h
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

#ifndef _LCD_H_
#define _LCD_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "define.h"


//lcd指示
#define LCD_DATA          		(8u)
#define LCD_WRB          		(9u)
#define LCD_CSB          		(10u)
#define LCD_POWER				(18u)

//---------------------LCD控制命令---------------------------------
#define SYSDIS 				0x00 //关闭系统振荡器
#define SYSEN 				0x01 //打开系统振荡器
#define LCDOFF 				0x02 //关闭LCF偏压
#define LCDON 				0x03 //打开LCF偏压
#define RC256 				0x18 //使用内部时钟
#define BIAS_1_3			0x29 //LCD1/3偏压设置
#define	nop()				nrf_delay_us(1)
#define LCD_DRIVE_DELAY()	do{ __nop();__nop(); }while(0)


//------------------LCD显示枚举类型--------------------------------
typedef enum 
{
	SEND_RESULT_CLEAR		= 0X00,
	SEND_RESULT_OK			= 0X01,
	SEND_RESULT_FAIL		= 0X02
}lcd_send_result_enum;		

typedef enum 
{
	JUDGE_CLEAR		= 0X00,
	JUDGE_TRUE		= 0X01,
	JUDGE_FALSE		= 0X02
}lcd_judge_enum;		

typedef enum 
{
	BATTERY_LEVEL_0 = 0X00,
	BATTERY_LEVEL_1 = 0X01,
	BATTERY_LEVEL_2 = 0X02,
	BATTERY_LEVEL_3 = 0X03
}lcd_battery_level_enum;				

typedef enum 
{
	RSSI_0 = 0X00,
	RSSI_1 = 0X01,
	RSSI_2 = 0X02,
	RSSI_3 = 0X03,
	RSSI_4 = 0X04
}lcd_signal_level_enum;				



typedef struct 
{
	bool				update_flg;						//刷新标志
	
	void				(*on)(void);
	void				(*off)(void);
	void				(*sleep)(void);				
	void				(*wakeup)(void);
	
	void				(*timer_start)(void);
	void				(*timer_stop)(void);				
	void				(*timer_handler)(void * p_context);
	
	void                (*update)(void);				//LCD刷新函数
	
	void				(*battery)(uint8_t tmp);		//电池电量
	void				(*signal)(uint8_t tmp);			//信号强度
	void				(*send_result)(uint8_t tmp);	//发送结果
	void				(*judge)(uint8_t tmp);			//判断题答案
	void				(*choice)(uint8_t tmp);			//选择题答案
	void				(*student_id)(uint16_t tmp);	//学号
	void				(*question_num)(uint8_t tmp);	//题号
	void				(*grade_value)(uint8_t tmp);	//评分值
	void                (*clear_segment)(void);	
}lcd_struct_t;


extern lcd_struct_t				*LCD;

void LCD_init(void);
void LCD_display_updata( void );
void LCD_diplay_test(void);

void LCD_diplay_dot( void );
void LCD_clear_dot( void );
void LCD_diplay_segment(uint8_t num, uint8_t data);
void LCD_clear_segment(void);

#endif	//_LCD_H_
 




