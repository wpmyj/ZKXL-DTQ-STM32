/** @file rtc_dispaly.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V5.18a.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2016.07.14
 * $Revision: 	V1000
 *
 */

 
#ifndef  _RTC_DISPLAY_H_
#define  _RTC_DISPLAY_H_

#include "lcd.h"
#include "rtc.h"


typedef struct                        //RTC½á¹¹
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}	RTC_TypeDef;

extern RTC_TypeDef        rtc_time;


void rtc_handler(void);
void read_rtc_data(void);
void LCD_display_time(void);
void LCD_display_date(void);
void adjust_rtc_minute(void);
void adjust_rtc_hour(void);
void adjust_rtc_day(void);
void adjust_rtc_month(void);
void adjust_rtc_year(void);
void adjust_rtc_week(void);

void LCD_display_minute(void);
void LCD_display_hour(void);
void LCD_display_day(void);
void LCD_display_month(void);
void LCD_display_year(void);
void LCD_display_week(void);




#endif
