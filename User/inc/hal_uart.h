/**
  ******************************************************************************
  * @file   	hal_uart.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform hal_uart functions's declaritions
  ******************************************************************************
  */
#ifndef _HAL_UART_H_
#define _HAL_UART_H_

#include "variable.h"

void hal_uart_clr_tx(uint8_t tx_index);
void hal_uart_clr_rx(void);
void CtrUartSendTask(uint8_t *ptr,uint8_t len);
void app_debuglog_dump(uint8_t * p_buffer, uint32_t len);
void app_debuglog_dump_no_space(uint8_t * p_buffer, uint32_t len);

#endif //_HAL_UART_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/

