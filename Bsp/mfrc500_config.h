/**
  ******************************************************************************
  * @file   	mfrc500_config.h
  * @author  	Sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.12.29
  * @brief   	hal config for 13.56Mhz moulde
  ******************************************************************************
  */
#ifndef __MFRC500_CONFIG_H_
#define __MFRC500_CONFIG_H_

#include "main.h"

#define ENABLE_MF1702NL_DEBUG_LOG

#ifdef 	ENABLE_MF1702NL_DEBUG_LOG
#define DEBUG_MF1702NL_LOG							     DEBUG_LOG
#else
#define DEBUG_MF1702NL_LOG(...)
#endif

/*****************************************************************************
*端口定义
*****************************************************************************/
#define MFRC500_PD_Port         GPIOC
#define	MFRC500_PD_Pin          GPIO_Pin_6
#define MFRC500_IRQ_Port        GPIOC
#define	MFRC500_IRQ_Pin         GPIO_Pin_7
#define MFRC500_ALE_Port        GPIOC
#define	MFRC500_ALE_Pin         GPIO_Pin_8
#define MFRC500_CS_Port         GPIOC
#define	MFRC500_CS_Pin          GPIO_Pin_9
#define MFRC500_WR_Port         GPIOA
#define	MFRC500_WR_Pin          GPIO_Pin_8
#define MFRC500_RD_Port         GPIOA
#define	MFRC500_RD_Pin          GPIO_Pin_15
#define MFRC500_DATA_Port       GPIOB

#define MFRC500_RD_PORT         GPIOA
#define MFRC500_WR_PORT         GPIOA
#define MFRC500_CTL_PORT        GPIOC


/*****************************************************************************
*数据类型定义
*****************************************************************************/
#define FIFO_LENGTH       	     64			// FIFO size = 64byte
#define MFMesssge_LEN            100

typedef struct
{
	uint8_t MfCommand;
	uint8_t MfLength;
	uint8_t MfData[MFMesssge_LEN];
	uint8_t mfcurrent;			       // 当前数据下标 
}MFMessage_Typedef;

#endif
/**************************************END OF FILE****************************/
