/** @file rtc.h
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
 * $Date: 		2016.06.20
 * $Revision: 	V1000
 *
 */
  
#ifndef	_RTC_H_
#define _RTC_H_
 
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "lcd.h"
#include "define.h"

//RTC IIC
#define RTC_I2C_INT         	(25u)
#define RTC_I2C_SDA          	(26u)
#define RTC_I2C_SCL         	(27u)

#define RTC_I2C_WRITE_ADDR								(0xA2)				//����IIC�豸д��ַ
#define RTC_I2C_READ_ADDR								(0xA3)				//����IIC�豸����ַ

#define RTC_HOUR                         (0x04)       //����hour�Ĵ�����ַ
#define RTC_DAY                         (0x05)       //����day�Ĵ�����ַ
#define RTC_WEEK                         (0x06)       //����week�Ĵ�����ַ
#define RTC_MONTH                         (0x07)       //����month�Ĵ�����ַ

typedef struct                        //RTC�ṹ
{
	uint8_t year;			//�����ֲ� uint16_t ��Ϊ uint8_t ��by lj
	uint8_t month;
	uint8_t day;
	uint8_t week;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
}RTC_TypeDef;

typedef struct 
{
	bool				update_flg;					//ˢ�±�־
	
	void 				(*update)(void);
	void				(*time)(void);
	void				(*date)(void);
	void 				(*read_data)(void);
	
	void				(*sleep)(void);
	void				(*wakeup)(void);
}rtc_struct_t;

extern rtc_struct_t				*RTC;
extern RTC_TypeDef       		rtc_time;

void RTC_Init( void );
void RTC_IRQInit(void);
void RTC_display_update(void);

bool RTC_WriteNReg(uint8_t reg, uint8_t *data, uint8_t len);
#endif	//_RTC_H_

