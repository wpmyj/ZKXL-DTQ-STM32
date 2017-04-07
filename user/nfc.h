/** @file nfc.h
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
 
#ifndef	_NFC_H_
#define	_NFC_H_
 
//#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "iic.h"
#include "nrf_delay.h"
#include "m24sr_cmd.h"
#include "define.h"

/* ---------------------- status code ----------------------------------------*/
#define M24SR_ACTION_COMPLETED								(0x9000)			//返回命令执行完成状态
/* Status and error code -----------------------------------------------------*/	 
#define M24SR_STATUS_SUCCESS								(0x0000)
#define M24SR_ERROR_DEFAULT									(0x0010)
#define M24SR_ERROR_I2CTIMEOUT								(0x0011)
#define M24SR_ERROR_CRC										(0x0012)
#define M24SR_ERROR_NACK									(0x0013)
#define M24SR_ERROR_PARAMETER								(0x0014) 
#define M24SR_ERROR_NBATEMPT								(0x0015) 
#define M24SR_ERROR_NOACKNOWLEDGE							(0x0016)

#define M24SR_I2C_TIMEOUT									(0x0FFF)			//定义IIC超时时间
#define M24SR_I2C_WRITE_ADDR								(0xAC)				//定义IIC设备写地址
#define M24SR_I2C_READ_ADDR									(0xAD)				//定义IIC设备读地址

#define ASK_FOR_SESSION										(0x0000)
#define TAKE_SESSION										(0xFFFF) 
#define NDEF_SESSION_CLOSED									(0xDEADBEEF)

/* GPO state structure -------------------------------------------------------*/
typedef enum{
	HIGH_IMPEDANCE = 0,
	SESSION_OPENED,
	WIP,
	I2C_ANSWER_READY,
	INTERRUPT,
	STATE_CONTROL
}M24SR_GPO_MGMT;

typedef enum{
	IIC_NO_PROTECT = 0,
	IIC_PROTECT	
}M24SR_IIC_MGMT;

typedef struct
{
	uint16_t NumberCCByte;
	uint8_t Version;
	uint16_t MaxReadByte;
	uint16_t MaxWriteByte;
	uint8_t TField;
	uint8_t LField;
	uint16_t FileID;
	uint16_t NDEFFileMaxSize;
	uint8_t ReadAccess;
	uint8_t WriteAccess;
}sCCFileInfo;

uint16_t M24SR_SendI2Ccommand(uint8_t *buf, uint8_t len);
uint16_t M24SR_ReceiveI2Cbuffer(uint8_t *buf, uint8_t len);
uint16_t M24SR_PollI2C ( void );
uint16_t M24SR_ManageI2CGPO( uint8_t GPO_I2Cconfig);
uint16_t M24SR_ManageRFGPO( uint8_t GPO_RFconfig);
void M24SR_GetUID( uint8_t* UIDBuf);
uint16_t M24SR_ManageIICProtect( uint8_t IICconfig);
uint16_t M24SR_ReadData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData);
uint16_t M24SR_WriteData ( uint16_t Offset , uint16_t DataSize , uint8_t* pData);
uint16_t M24SR_OpenNDEFSession ( uint16_t NDEF_fileID, uint16_t Priority );
uint16_t M24SR_CloseNDEFSession ( uint16_t NDEF_fileID );
uint16_t TT4_Init (void);
uint16_t TT4_ReadNDEF(uint8_t *pNDEF);			//13.56M读flash函数
uint16_t TT4_WriteNDEF(uint8_t *pNDEF);			//13.56M写flash函数

void NFC_init(void);


#endif	//_NFC_H_

