/** @file rtc_display.c
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V5.18a.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.07.14
 * $Revision: 	V1000
 *
 */

#include "rtc_display.h"


uint8_t Second_L,Second_H,Minute_L,Minute_H,Hour_L,Hour_H;
uint8_t Day_L,Day_H,Month_L,Month_H,Year_L,Year_H;

RTC_TypeDef        rtc_time;
	
/******************************************************************************
  @函数:rtc_handler
  @描述:
  @输入:
  @输出:
  @调用:
******************************************************************************/
void rtc_handler(void)
{
	read_rtc_data();    
	LCD_display_time();
}

/******************************************************************************
  @函数:read_rtc_data
  @描述:读取RTC中的时间信息到rtc_time结构体中
  @输入:
  @输出:
  @调用:
******************************************************************************/
void read_rtc_data(void)        
{
	uint8_t temp_data[7] = {0};

	RTC_ReadNReg(0x02, temp_data, 7);

	rtc_time.second=temp_data[0];
	rtc_time.minute=temp_data[1];
	rtc_time.hour=temp_data[2];
	rtc_time.day=temp_data[3];
	rtc_time.week=temp_data[4];
	rtc_time.month=temp_data[5];
	rtc_time.year=temp_data[6];
	
	//printf("read rtc: %02X:%02X:%02X  \n",temp_data[2],temp_data[1],temp_data[0]);
}

/******************************************************************************
  @函数:read_rtc_data
  @描述:读取RTC中的时间信息到rtc_time结构体中
  @输入:
  @输出:
  @调用:
******************************************************************************/
void LCD_display_minute(void)
{
	Minute_L=rtc_time.minute& 0x0f;
	Minute_H=(rtc_time.minute & 0xf0)>>4;
	LCD_diplay_segment(3,Minute_H);
	LCD_diplay_segment(4,Minute_L);	
}

void LCD_display_hour(void)
{
	Hour_L=rtc_time.hour & 0x0f;
	Hour_H=(rtc_time.hour & 0xf0)>>4;
	
	LCD_diplay_segment(1,Hour_H);
	LCD_diplay_segment(2,Hour_L);
}

void LCD_display_day(void)
{
	Day_L=rtc_time.day & 0x0f;
	Day_H=(rtc_time.day & 0xf0)>>4;
	
	LCD_diplay_segment(3,Day_H);
	LCD_diplay_segment(4,Day_L);
}

void LCD_display_month(void)
{
	Month_L=rtc_time.month& 0x0f;
	Month_H=(rtc_time.month & 0xf0)>>4;
	
	LCD_diplay_segment(1,Month_H);
	LCD_diplay_segment(2,Month_L);
}

void LCD_display_year(void)
{
	Year_L=rtc_time.year & 0x0f;
	Year_H=(rtc_time.year & 0xf0)>>4;

	LCD_diplay_segment(5,Year_H);
	LCD_diplay_segment(6,Year_L );
}


void LCD_display_time(void)
{
//	static uint8_t dot_display = 0;
	Second_L=rtc_time.second & 0x0f;
	Second_H=(rtc_time.second & 0xf0)>>4;
	Minute_L=rtc_time.minute& 0x0f;
	Minute_H=(rtc_time.minute & 0xf0)>>4;
	Hour_L=rtc_time.hour & 0x0f;
	Hour_H=(rtc_time.hour & 0xf0)>>4;
	
//	printf("change to time: %d%d:%d%d:%d%d \n",Hour_H,Hour_L,Minute_H,Minute_L,Second_H,Second_L);

//	if(dot_display)
//	{
		LCD_diplay_dot();
//		dot_display = 0;
//	}
//	else
//	{
//		LCD_clear_dot();
//		dot_display = 1;
//	}
	
	LCD_diplay_segment(4,Hour_H);
	LCD_diplay_segment(3,Hour_L);
	LCD_diplay_segment(2,Minute_H);
	LCD_diplay_segment(1,Minute_L);	
//	LCD_diplay_segment(6,Second_H);
//	LCD_diplay_segment(5,Second_L );
}
void LCD_display_date(void)
{
	Day_L=rtc_time.day & 0x0f;
	Day_H=(rtc_time.day & 0xf0)>>4;
	Month_L=rtc_time.month& 0x0f;
	Month_H=(rtc_time.month & 0xf0)>>4;
	Year_L=rtc_time.year & 0x0f;
	Year_H=(rtc_time.year & 0xf0)>>4;


	LCD_diplay_segment(1,Month_H);
	LCD_diplay_segment(2,Month_L);
	LCD_diplay_segment(3,Day_H);
	LCD_diplay_segment(4,Day_L);	
	LCD_diplay_segment(5,Year_H);
	LCD_diplay_segment(6,Year_L );
	//LCD_diplay_dot(6);
}

void adjust_rtc_minute(void)
{
	uint8_t temp=0,temp1=0;

		temp=rtc_time.minute & 0x0f;
		temp1=(rtc_time.minute & 0xf0)>>4;
	if(16==Key_scan(true))    //按下题键加1
	{
		nrf_delay_ms(15);
		if(16==Key_scan(true))
		{
			++temp;

			if(temp>9)
			{
				temp=0;

				if(temp1<5)
				{
					temp1 +=1;
					
					rtc_time.minute=(temp1<<4);
				
				}		
				else rtc_time.minute=0;
				
				RTC_WriteReg( 0x03, rtc_time.minute);
				read_rtc_data();
				LCD_display_minute();
			}
		
			else
			{
				++rtc_time.minute;
				RTC_WriteReg( 0x03, rtc_time.minute);
				read_rtc_data();
				LCD_display_minute();
			}
		}					
	}
	else if(15==Key_scan(true))   //按上题键减1
	{
		nrf_delay_ms(15);
		if(15==Key_scan(true))
		{
			--temp;
			if(temp>0 )
			{
				--rtc_time.minute;
				RTC_WriteReg( 0x03, rtc_time.minute);
				read_rtc_data();
				LCD_display_minute();
			}	
			else
				
			{
				temp=9;
				if(temp1!=0)
				{
					--temp1;
					rtc_time.minute=(temp1<<4)+temp;
					RTC_WriteReg( 0x03, rtc_time.minute);
					read_rtc_data();
					LCD_display_minute();
				}
				else
				{
					rtc_time.minute=0x59;
					RTC_WriteReg( 0x03, rtc_time.minute);
					read_rtc_data();
					LCD_display_minute();
				}

			}			
		}
	}			
}

void adjust_rtc_hour(void)
{
	
}
void adjust_rtc_day(void)
{
	
}
void adjust_rtc_month(void)
{
	
}
void adjust_rtc_year(void)
{
	
}
void adjust_rtc_week(void)
{
	
}
