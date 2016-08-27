/**
  ******************************************************************************
  * @file   	nrf.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	include nrf header files
  ******************************************************************************
  */
#ifndef _NRF_H_
#define _NRF_H_

#include <stdint.h>
#include "variable.h"

void NRF_Module_Set(void);
void nrf24AllRegResetVal_nrf1(void);
void nrf24AllRegResetVal_nrf2(void);
void nrf24InitConf_NRF1( void );
void nrf24InitConf_NRF2( void );
void nrf24InitConf_NRF2_SendMode(void);
void nrf24AddtoAck(const uint8_t *puBuf, uint8_t uWidth);
void nrf24SendPacket(const uint8_t *puBuf, uint8_t uWidth);
void nrf24SendPacket_NRF2(const uint8_t *puBuf, uint8_t uWidth);

#endif //_NRF_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/



