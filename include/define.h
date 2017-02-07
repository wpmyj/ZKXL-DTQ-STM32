/**
  ******************************************************************************
  * @file   	define.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	macro define
  ******************************************************************************
  */
#ifndef _DEFINE_H_
#define _DEFINE_H_

/* Defines ------------------------------------------------------------------*/
//#define OPEN_SILENT_MODE
#define ENABLE_WATCHDOG					(1)
//#define ENABLE_OUTPUT_MODE_NORMOL		
//#define ENABLE_DEBUG_LOG			
//#define ENABLE_RF_DATA_SHOW   


#define DISABLE_ALL_IRQ()  			__set_PRIMASK(1);
#define ENABLE_ALL_IRQ()   			__set_PRIMASK(0);

#define UART_BAUD								1152000

#ifdef 	ENABLE_DEBUG_LOG
#define DebugLog								printf
#else
#define DebugLog(...)
#endif
/* Uart Message configuration */
#define UART_NBUF								(240)
#define RF_NBUF									(UART_NBUF)

#endif //_DEFINE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/
