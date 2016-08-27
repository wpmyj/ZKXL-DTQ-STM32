/**
  ******************************************************************************
  * @file   	delay.h
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.27
  * @brief   	delay function
  ******************************************************************************
  */

#ifndef __DELAY_H_
#define __DELAY_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Private functions ---------------------------------------------------------*/
void SysClockInit(void);
void TimingDelay_Decrement(void);

void Delay2us(uint32_t times);
void Delay3us(void);
void Delay10us(uint16_t times);
void DelayMs(__IO uint32_t nTime);

#endif //__DELAY_H_
/**************************************END OF FILE****************************/

