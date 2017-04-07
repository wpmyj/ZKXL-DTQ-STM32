/** @file iic.c
 *
 * @author 
 *
 * @compiler This program has been tested with Keil MDK V5.18a.
 *
 * @copyright
 * Copyright (c) 2016 ZKXL. All Rights Reserved.
 *
 * $Date: 		2016.06.20
 * $Revision: 	V1000
 *
 */
 
 
/*-------------------------------- Include File -----------------------------------------*/
#include "rtc.h"
/*-------------------------------- Macro definition -------------------------------------*/
#define RTC_I2C_DELAY()						{do{nrf_delay_us(5);}while(0);}
#define RTC_I2C_SDA_HIGH()					{nrf_gpio_pin_set(RTC_I2C_SDA);}
#define RTC_I2C_SDA_LOW()					{nrf_gpio_pin_clear(RTC_I2C_SDA);}
#define RTC_I2C_SCL_HIGH()					{nrf_gpio_pin_set(RTC_I2C_SCL);}
#define RTC_I2C_SCL_LOW()					{nrf_gpio_pin_clear(RTC_I2C_SCL);}

/*-------------------------------- Local Variable ----------------------------------------*/
uint8_t Second_L,Second_H,Minute_L,Minute_H,Hour_L,Hour_H;
uint8_t Day_L,Day_H,Month_L,Month_H,Year_L,Year_H;

rtc_struct_t				rtc_struct;
RTC_TypeDef       			 rtc_time;

/*-------------------------------- Gocal Variable ----------------------------------------*/
rtc_struct_t				*RTC = &rtc_struct;

/*-------------------------------- Function Declaration ----------------------------------*/
static void RTC_I2C_Start(void);
static void RTC_I2C_Stop(void);
static void RTC_Send_Ack(uint8_t ack);
static uint8_t RTC_Check_Ack(void);
static bool RTC_Write_Byte(uint8_t dat);
static uint8_t RTC_ReadByte(uint8_t send_ack);
static bool RTC_WriteReg(uint8_t reg, uint8_t data);
static uint8_t RTC_ReadReg( uint8_t reg );
//static bool RTC_WriteNReg(uint8_t reg, uint8_t *data, uint8_t len);
static bool RTC_ReadNReg( uint8_t reg ,uint8_t *data, uint8_t len);
static void RTC_display_date(void);
static void read_rtc_data(void);
static void RTC_sleep(void);
static void RTC_wakeup(void);
static void RTC_gpio_config(bool tmp);



/*-------------------------------- Revision record ---------------------------------------*/


/*********************************************************************************
* ��	�ܣ�void RTC_I2C_Start(void)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static void RTC_I2C_Start(void)
{
	RTC_I2C_SDA_HIGH();
	RTC_I2C_SCL_HIGH();
	RTC_I2C_DELAY();
	
	RTC_I2C_SDA_LOW();
	RTC_I2C_DELAY();
	
	RTC_I2C_SCL_LOW();
	RTC_I2C_DELAY();
}

/*********************************************************************************
* ��	�ܣ�void RTC_I2C_Stop(void)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static void RTC_I2C_Stop(void)
{
	RTC_I2C_SCL_LOW();
	RTC_I2C_SDA_LOW();
	RTC_I2C_DELAY();
	
	RTC_I2C_SCL_HIGH();
	RTC_I2C_DELAY();
	
	RTC_I2C_SDA_HIGH();
	RTC_I2C_DELAY();
}

/*********************************************************************************
* ��	�ܣ�void RTC_Send_Ack(uint8_t ack)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static void RTC_Send_Ack(uint8_t ack)
{
	RTC_I2C_SCL_LOW();
	RTC_I2C_DELAY();
	
	nrf_gpio_cfg_output(RTC_I2C_SDA);							//��SDA����Ϊ���
	if(ack)
	{	RTC_I2C_SDA_HIGH();	}
	else
	{	RTC_I2C_SDA_LOW();	}
	RTC_I2C_DELAY();
	
	RTC_I2C_SCL_HIGH();
	RTC_I2C_DELAY();
	
	RTC_I2C_SCL_LOW();
	RTC_I2C_DELAY();
}

/*********************************************************************************
* ��	�ܣ�uint8_t RTC_Check_Ack(void)
* ��    ��: NULL
* ��	�أ�ACK״̬
* ��	ע��NULL
*********************************************************************************/
static uint8_t RTC_Check_Ack(void)
{
	uint8_t tempbit;

	RTC_I2C_SCL_LOW();
	RTC_I2C_SDA_HIGH();
	RTC_I2C_DELAY();
	
	nrf_gpio_cfg_input(RTC_I2C_SDA, NRF_GPIO_PIN_PULLUP);		//��SDA����Ϊ����
	RTC_I2C_DELAY();
	
	RTC_I2C_SCL_HIGH();
	RTC_I2C_DELAY();
	tempbit = nrf_gpio_pin_read(RTC_I2C_SDA);
	RTC_I2C_SCL_LOW();
	RTC_I2C_DELAY();
	
	nrf_gpio_cfg_output(RTC_I2C_SDA);							//��SDA����Ϊ���
	
	return tempbit; 											//����ACK 1 or 0
}

/*********************************************************************************
* ��	�ܣ�void RTC_Write_Byte(uint8_t dat)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static bool RTC_Write_Byte(uint8_t dat)
{
	uint8_t bitCnt = 0 ;
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		RTC_I2C_SCL_LOW();
		RTC_I2C_DELAY();
		if((dat & 0x80) == 0x80) 												//if the MSb is 1
		{
			RTC_I2C_SDA_HIGH();
		}
		else
		{
			RTC_I2C_SDA_LOW();
		}
		RTC_I2C_DELAY();
		RTC_I2C_SCL_HIGH();
		RTC_I2C_DELAY();
		dat <<= 1 ;
	}
	
	if(RTC_Check_Ack())
		return false;
	else
		return true;
	
}

/*********************************************************************************
* ��	�ܣ�uint8_t RTC_ReadByte(uint8_t send_ack)
* ��    ��: NULL
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static uint8_t RTC_ReadByte(uint8_t send_ack)
{
	uint8_t tempbit = 1 ;
	uint8_t temp = 0 ;
	uint8_t bitCnt ;
	
	RTC_I2C_SDA_HIGH(); 															//�ͷ�SDA����ʼ��������
	RTC_I2C_DELAY();
	nrf_gpio_cfg_input(RTC_I2C_SDA, NRF_GPIO_PIN_PULLUP);							//��SDA����Ϊ����
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		RTC_I2C_SCL_LOW(); 
		RTC_I2C_DELAY();
		RTC_I2C_DELAY();
		RTC_I2C_SCL_HIGH();		
		tempbit = nrf_gpio_pin_read(RTC_I2C_SDA);
		if(tempbit)
			temp |= 0x01 ;
		else
			temp &= 0xfe ;
		RTC_I2C_DELAY();
		if(bitCnt<7)
			temp <<= 1 ;
	}
	
	RTC_I2C_SCL_LOW();
	nrf_gpio_cfg_output(RTC_I2C_SDA);												//��SDA����Ϊ���
	
	RTC_Send_Ack(send_ack);
	
	return (temp) ;
}


/*********************************************************************************
* ��	�ܣ�bool RTC_WriteReg(uint8_t reg, uint8_t data)
* ��    ��: reg		�Ĵ�����ַ
			data	д�������
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static bool RTC_WriteReg(uint8_t reg, uint8_t data)
{
	bool state = false;
	
	RTC_I2C_Start();
	if(RTC_Write_Byte( RTC_I2C_WRITE_ADDR ))
	{	
		if(RTC_Write_Byte( reg ))
		{
			if(RTC_Write_Byte( data ))
				state = true;
			else
				state = false;
		}
		else
			state = false;
	}
	
	RTC_I2C_Stop();
	
	return state;
}

/*********************************************************************************
* ��	�ܣ�uint8_t RTC_ReadReg( uint8_t reg )
* ��    ��: reg		�Ĵ�����ַ
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static uint8_t RTC_ReadReg( uint8_t reg )
{
	bool state = false;
	uint8_t temp = 0;
	
	RTC_I2C_Start();
	
	if(RTC_Write_Byte( RTC_I2C_WRITE_ADDR ))
	{
		if(RTC_Write_Byte( reg	))
			state = true;
		else
			state = false;
	}

	if(state)
	{
		RTC_I2C_Start();

		if(RTC_Write_Byte( RTC_I2C_READ_ADDR ))
		{
			temp = RTC_ReadByte( 1 );
			if(RTC_HOUR==reg)
			{
				temp &= ~(1<<6);
			}
			if(RTC_DAY==reg)
			{
				temp &= ~(1<<6);
			}
			if(RTC_WEEK==reg)
			{
				temp &= ~(0xF8);
			}
			if(RTC_MONTH==reg)
			{
				temp &= ~(0x03<<5);
			}
		}
	}
	RTC_I2C_Stop();
	return temp;
}



/*********************************************************************************
* ��	�ܣ�bool RTC_WriteNReg(uint8_t reg, uint8_t *data, uint8_t len)
* ��    ��: reg		�Ĵ�����ַ
			data	д�������
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
bool RTC_WriteNReg(uint8_t reg, uint8_t *data, uint8_t len)
{
	bool state = false;
	uint8_t i;
	RTC_I2C_Start();
	if(RTC_Write_Byte( RTC_I2C_WRITE_ADDR ))
	{	
		if(RTC_Write_Byte( reg ))
		{
			for(i = 0; i < len; i ++)
			{
				if(!RTC_Write_Byte( data[i] ))
				{
					state = false;
					break;
				}
			}
			state = true;
		}
		else
			state = false;
	}
	
	RTC_I2C_Stop();
	
	return state;
}

/*********************************************************************************
* ��	�ܣ�bool RTC_ReadNReg( uint8_t reg ,uint8_t *data, uint8_t len)
* ��    ��: reg		�Ĵ�����ַ
* ��	�أ�NULL
* ��	ע��NULL
*********************************************************************************/
static bool RTC_ReadNReg( uint8_t reg ,uint8_t *data, uint8_t len)
{
	bool state = false;
	uint8_t i = 0;

	RTC_I2C_Start();

	if(RTC_Write_Byte( RTC_I2C_WRITE_ADDR ))
	{
		if(RTC_Write_Byte( reg	))
		state = true;
		else
		state = false;
	}
	
	if(state)
	{
		RTC_I2C_Start();
		if(RTC_Write_Byte( RTC_I2C_READ_ADDR ))
		{
			for(i = 0; i < len - 1; i ++)
			{
				data[i] = RTC_ReadByte( 0 );
				if(RTC_HOUR==i+reg)
				{
					data[i] &= ~(1<<6);
				}
				if(RTC_DAY==i+reg)
				{
					data[i] &= ~(1<<6);
				}
				if(RTC_WEEK==i+reg)
				{
					data[i] &= ~(0xF8);
				}
				if(RTC_MONTH==i+reg)
				{
					data[i] &= ~(0x03<<5);
				}
			}
			data[len - 1] = RTC_ReadByte( 1 );
			state = true;
		}
		else
		state = false;
	}

	RTC_I2C_Stop();

	return state;
}

//��ʾRTC����
static void RTC_display_date(void)
{
	Day_L=rtc_time.day & 0x0f;
	Day_H=(rtc_time.day & 0xf0)>>4;
	Month_L=rtc_time.month& 0x0f;
	Month_H=(rtc_time.month & 0xf0)>>4;
	Year_L=rtc_time.year & 0x0f;
	Year_H=(rtc_time.year & 0xf0)>>4;
}

//��ʾRTCʱ��
static void RTC_display_time(void)
{
	static uint8_t dot_display = 0;
	
	Second_L=rtc_time.second & 0x0f;
	Second_H=(rtc_time.second & 0xf0)>>4;
	Minute_L=rtc_time.minute& 0x0f;
	Minute_H=(rtc_time.minute & 0xf0)>>4;
	Hour_L=rtc_time.hour & 0x0f;
	Hour_H=(rtc_time.hour & 0xf0)>>4;
	
	if(dot_display)
	{
		LCD_diplay_dot();
		dot_display = 0;
	}
	else
	{
		LCD_clear_dot();
		dot_display = 1;
	}
	
	LCD_diplay_segment(4,Hour_H);
	LCD_diplay_segment(3,Hour_L);
	LCD_diplay_segment(2,Minute_H);
	LCD_diplay_segment(1,Minute_L);	
//	LCD_diplay_segment(6,Second_H);			//����ʾ�룬��λ��������ʾѧ��
//	LCD_diplay_segment(5,Second_L);
}


/******************************************************************************
  @����:read_rtc_data
  @����:��ȡRTC�е�ʱ����Ϣ��rtc_time�ṹ����
  @����:
  @���:
  @����:
******************************************************************************/
static void read_rtc_data(void)        
{
	uint8_t temp_data[7] = {0};

	RTC_ReadNReg(0x02, temp_data, 7);	//��ȡʱ�������
//	RTC_ReadNReg(0x02, temp_data, 3);	//����ȡʱ����Ϣ��������ʱ�ò���

	rtc_time.second=temp_data[0];
	rtc_time.minute=temp_data[1];
	rtc_time.hour=temp_data[2];
	rtc_time.day=temp_data[3];
	rtc_time.week=temp_data[4];
	rtc_time.month=temp_data[5];
	rtc_time.year=temp_data[6];
	
}


static void RTC_gpio_config(bool tmp)
{
	uint8_t i;
	
	if(tmp)
	{
		//I2C GPIO����ΪĬ��״̬
		for(i = RTC_I2C_INT;i < 3;i++)
		{
			NRF_GPIO->PIN_CNF[i] = (GPIO_PIN_CNF_SENSE_Disabled << GPIO_PIN_CNF_SENSE_Pos)
								   | (GPIO_PIN_CNF_DRIVE_S0S1 << GPIO_PIN_CNF_DRIVE_Pos)
								   | (GPIO_PIN_CNF_PULL_Disabled << GPIO_PIN_CNF_PULL_Pos)
								   | (GPIO_PIN_CNF_INPUT_Disconnect << GPIO_PIN_CNF_INPUT_Pos)
								   | (GPIO_PIN_CNF_DIR_Input << GPIO_PIN_CNF_DIR_Pos);
		}
	}
	else
	{
		//��������I2C����
		//nrf_gpio_cfg_sense_input(RTC_I2C_INT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
		nrf_gpio_pin_set(RTC_I2C_SCL);
		nrf_gpio_pin_set(RTC_I2C_SDA);	
		nrf_gpio_cfg_output(RTC_I2C_SCL);
		nrf_gpio_cfg_output(RTC_I2C_SDA);
	}
}


//RTC��������Ϊ����̬�����ٵ�������
static void RTC_sleep(void)
{
//	RTC_gpio_config(false);
	
}

//RTC�����������ÿɹ�I2Cʹ��
static void RTC_wakeup(void)
{
//	RTC_gpio_config(true);
}


void RTC_Init( void )
{
	bool rtc_init_flg = false;		
	
	uint8_t rtc_data[7];
	
	/* RTC GPIO ��ʼ��*/
	//nrf_gpio_cfg_sense_input(RTC_I2C_INT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	nrf_gpio_pin_set(RTC_I2C_SCL);
	nrf_gpio_pin_set(RTC_I2C_SDA);	
	nrf_gpio_cfg_output(RTC_I2C_SCL);
	nrf_gpio_cfg_output(RTC_I2C_SDA);
	
	//��ֹRTC�ж��������
	RTC_WriteReg(0x0D, 0x00);			
	
	/* RTC �ṹ���ʼ�� */
	RTC->update_flg = true;
	RTC->wakeup		= RTC_wakeup;
	RTC->sleep  	= RTC_sleep;
	RTC->time		= RTC_display_time;
	RTC->date		= RTC_display_date;
	RTC->update     = RTC_display_update;
	RTC->read_data  = read_rtc_data;
	
	//��ȡ��ǰRTC���ݣ��ж��Ƿ�������
	//����������Ϊʱ��û��У׼������RTC����ֵ
	read_rtc_data();
	Second_L=rtc_time.second & 0x0f;
	Second_H=(rtc_time.second & 0xf0)>>4;
	Minute_L=rtc_time.minute& 0x0f;
	Minute_H=(rtc_time.minute & 0xf0)>>4;
	Hour_L=rtc_time.hour & 0x0f;
	Hour_H=(rtc_time.hour & 0xf0)>>4;
	Day_L=rtc_time.day & 0x0f;
	Day_H=(rtc_time.day & 0xf0)>>4;
	Month_L=rtc_time.month& 0x0f;
	Month_H=(rtc_time.month & 0xf0)>>4;
	Year_L=rtc_time.year & 0x0f;
	Year_H=(rtc_time.year & 0xf0)>>4;
	
	if (Month_H > 1)
		rtc_init_flg = true;
	
	if (Day_H > 3)
		rtc_init_flg = true;
	
	if (Hour_H > 5)
		rtc_init_flg = true;
	
	if (Minute_H > 5)
		rtc_init_flg = true;
	
	if (Second_H > 5)
		rtc_init_flg = true;
	
	if(rtc_init_flg)
	{
		rtc_init_flg = false;
		rtc_time.year = 16;
		rtc_time.month = 6;
		rtc_time.day = 21;
		rtc_time.week = 2;
		rtc_time.hour = 16;
		rtc_time.minute = 0;
		rtc_time.second = 0;
		rtc_data[0] = (rtc_time.second/10 << 4)|(rtc_time.second%10);
		rtc_data[1] = (rtc_time.minute/10 << 4)|(rtc_time.minute%10);
		rtc_data[2] = (rtc_time.hour/10 << 4)|(rtc_time.hour%10);
		rtc_data[3] = (rtc_time.day/10 << 4)|(rtc_time.day%10);
		rtc_data[4] = rtc_time.week;
		rtc_data[5] = (rtc_time.month/10 << 4)|(rtc_time.month%10)|0x00;
		rtc_data[6] = (rtc_time.year/10%10 << 4)|(rtc_time.year%10);
		RTC_WriteNReg(0x02,rtc_data, 7);
	}
}


void RTC_display_update(void)
{
#ifdef RTC_USE
	
	//RTC->update_flg��λ���Ҳ���������״̬���Ÿ���RTC��ʾ
	if(RTC->update_flg && (false == APP->answer_ing_flg))
	{
		
//		RTC->wakeup();				//��RTCǰ�������������ÿ���
		read_rtc_data();    
//		RTC->sleep();				//����RTC����������Ϊ����̬��������
		RTC_display_time();
		RTC->update_flg = false;
		
	}

#endif
}



void RTC_IRQInit(void)
{
//	uint8_t rtc_reg_buff[16];
	//RTC_I2C_Init();				//IO��ʼ��
	
//	rtc_reg_buff[0]  = 0x00;				//����״̬�Ĵ���1,STOP=0,�ж�ʱ������
//	rtc_reg_buff[1]  = 0x01;				//����״̬�Ĵ���2, ������ʱ���ж�
//	rtc_reg_buff[14] = 0x81;				//��������ʱ���Ĵ�����ʹ�ܶ�ʱ����Ƶ��64Hz
//	rtc_reg_buff[15] = 0x40;				//���ö�ʱ������������ֵΪ 64������ʱ 1s		
//	
//	RTC_WriteNReg(0x00,rtc_reg_buff, 16);
	
	RTC_WriteReg(0x00, 0x00);
	RTC_WriteReg(0x01, 0x01);
	RTC_WriteReg(0x0E, 0x81);
	RTC_WriteReg(0x0F, 0x40);
}

/*
	RTC�жϲ������ʣ�
	1.Ϊʲô���������1S�ĸߵ͵�ƽ���䣻
	2.64HZʱ�ߵ͵�ѹ���䣺n=64��ʾȫΪ��ѹ����
				n=128,1:1   n=192,2:1  n=255,3:1	
*/



