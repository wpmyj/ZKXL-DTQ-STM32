/** @file iic.h
 *
 * @author 
 *
 * @compiler This program has been tested with Keil C51 V7.50.
 *
 * @copyright
 * Copyright (c) 2012 ZKXL. All Rights Reserved.
 *
 * The information contained herein is confidential property of Nordic Semiconductor. The use,
 * copying, transfer or disclosure of such information is prohibited except by express written
 * agreement with Nordic Semiconductor.
 * @endcopyright
 *
 * $Date: 		2014.07.19
 * $Revision: 	V1000
 *
 */
 
#ifndef	_IIC_H_
#define _IIC_H_
 
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

//IIC
#define I2C_RF_EN         		(11u)
#define I2C_INT         		(12u)
#define I2C_SCL          		(13u)
#define I2C_SDA          		(14u)


void I2C_Init(void);
void I2C_Start(void);
void I2C_Stop(void);
void Send_Ack(void);
uint8_t Check_Ack(void);
void Write_Byte(uint8_t dat);
uint8_t ReadByte(void);

#endif	//_IIC_H_

