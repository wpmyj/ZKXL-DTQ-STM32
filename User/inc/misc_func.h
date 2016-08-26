/**
  ******************************************************************************
  * @file   	misc.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform misc functions's declaritions
  ******************************************************************************
  */
#ifndef _MISC_H_
#define _MISC_H_

#include "variable.h"

uint8_t XOR_Cal(uint8_t *data, uint16_t length);
uint16_t Crc16Cal(uint8_t const * dest, uint8_t length);


#endif //_MISC_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

