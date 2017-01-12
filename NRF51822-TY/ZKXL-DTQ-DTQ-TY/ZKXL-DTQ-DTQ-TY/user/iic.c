/** @file iic.c
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
 /* nfc所用IIC */
 
#include "iic.h"


#define I2C_DELAY()						{do{nrf_delay_us(6);}while(0);}
#define I2C_SDA_HIGH()					{nrf_gpio_pin_set(I2C_SDA);}
#define I2C_SDA_LOW()					{nrf_gpio_pin_clear(I2C_SDA);}
#define I2C_SCL_HIGH()					{nrf_gpio_pin_set(I2C_SCL);}
#define I2C_SCL_LOW()					{nrf_gpio_pin_clear(I2C_SCL);}

void I2C_Init(void)
{
	nrf_gpio_cfg_output(I2C_RF_EN);
	nrf_gpio_cfg_output(I2C_SCL);
	nrf_gpio_cfg_output(I2C_SDA);
	//nrf_gpio_cfg_sense_input(I2C_INT, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
	
	nrf_gpio_pin_clear(I2C_RF_EN);
	nrf_gpio_pin_set(I2C_SCL);
	nrf_gpio_pin_set(I2C_SDA);
}

/*********************************************************************************
* 功	能：void I2C_Start(void)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void I2C_Start(void)
{
	I2C_SDA_HIGH();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_DELAY();
}

/*********************************************************************************
* 功	能：void I2C_Stop(void)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void I2C_Stop(void)
{
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SDA_HIGH();
	I2C_DELAY();
	I2C_DELAY();
}

/*********************************************************************************
* 功	能：void Send_Ack(void)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void Send_Ack(void)
{
	I2C_SCL_LOW();
	I2C_DELAY();
	I2C_SDA_LOW();
	I2C_DELAY();
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SCL_LOW();
}

/*********************************************************************************
* 功	能：uint8_t Check_Ack(void)
* 输    入: NULL
* 返	回：ACK状态
* 备	注：NULL
*********************************************************************************/
uint8_t Check_Ack(void)
{
	uint8_t tempbit;

	I2C_SCL_LOW();
	I2C_SDA_HIGH();
	I2C_DELAY();
	nrf_gpio_cfg_input(I2C_SDA, NRF_GPIO_PIN_PULLUP);							//将SDA设置为输入
	I2C_DELAY();
	tempbit = nrf_gpio_pin_read(I2C_SDA);
	I2C_SCL_HIGH();
	I2C_DELAY();
	I2C_SCL_LOW();
	nrf_gpio_cfg_output(I2C_SDA);												//将SDA设置为输出
	if(tempbit==1)
	{
		return 0; 																//返回noACK
	}
	else
	{
		return 1; 																//返回ACK
	}
}

/*********************************************************************************
* 功	能：void Write_Byte(uint8_t dat)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
void Write_Byte(uint8_t dat)
{
	uint8_t bitCnt = 0 ;
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		I2C_SCL_LOW();
		I2C_DELAY();
		if((dat & 0x80) == 0x80) 												//if the MSb is 1
		{
			I2C_SDA_HIGH();
		}
		else
		{
			I2C_SDA_LOW();
		}
		I2C_DELAY();
		I2C_SCL_HIGH();
		I2C_DELAY();
		dat<<=1 ;
	}
}

/*********************************************************************************
* 功	能：uint8_t ReadByte(void)
* 输    入: NULL
* 返	回：NULL
* 备	注：NULL
*********************************************************************************/
uint8_t ReadByte(void)
{
	uint8_t tempbit = 1 ;
	uint8_t temp = 0 ;
	uint8_t bitCnt ;
	
	I2C_SDA_HIGH(); 															//释放SDA，开始接收数据
	I2C_DELAY();
	nrf_gpio_cfg_input(I2C_SDA, NRF_GPIO_PIN_PULLUP);							//将SDA设置为输入
	
	for(bitCnt=0; bitCnt<8; bitCnt++)
	{
		I2C_SCL_LOW(); 
		I2C_DELAY();
		I2C_DELAY();
		I2C_SCL_HIGH();		
		tempbit = nrf_gpio_pin_read(I2C_SDA);
		if(tempbit)
			temp |= 0x01 ;
		else
			temp &= 0xfe ;
		I2C_DELAY();
		if(bitCnt<7)
		temp <<= 1 ;
	}
	
	I2C_SCL_LOW();
	nrf_gpio_cfg_output(I2C_SDA);												//将SDA设置为输出
	return (temp) ;
}




