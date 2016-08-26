/**
  ******************************************************************************
  * @file   	init.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions's declaritions
  ******************************************************************************
  */
#ifndef _INIT_H_
#define _INIT_H_

#include "variable.h"
#include "define.h"
#include "crc16.h"


void Platform_Init(void);
void GetMcuid(void);
void ledInit(Led_TypeDef Led);
void ledOn(Led_TypeDef Led);
void ledOff(Led_TypeDef Led);
void ledToggle(Led_TypeDef Led);
void GPIOInit_BEEP(void);
void Usart1_Init(void);
void NVIC_Configuration_USART1(void);
void Usart2_Init(void);
void NVIC_Configuration_USART2(void);

void SPI_Init_NRF1(void);
void SPI_Init_NRF2(void);
void GPIOInit_SE2431L(void);
void SE2431L_LNA(void);
void SE2431L_Bypass(void);
void SE2431L_TX(void);
void GPIOInit_ShuaiJianQi(void);
void NVIC_Configuration_RFIRQ(void);
void GPIOInit_MFRC500(void);




#endif //_INIT_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

