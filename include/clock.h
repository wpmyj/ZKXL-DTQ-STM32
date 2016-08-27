/**
  ******************************************************************************
  * @file   	clock.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	clock config functions and delay functions's declaritions
  ******************************************************************************
  */
#ifndef _CLOCK_H_
#define _CLOCK_H_

#include <stdint.h>

void RCC_Configuration(void);
void Delay2us(uint32_t times);
void Delay3us(void);
void Delay10us(uint16_t times);
void Delay1Ms(void);
void Delay10Ms(void);
void Delay100Ms(void);
void Delay1s(void);


#endif //_CLOCK_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

