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

#define ENABLE_WATCHDOG					(1)
#define ENABLE_DEBUG_LOG				(1)

#define DISABLE_ALL_IRQ()  			__set_PRIMASK(1);
#define ENABLE_ALL_IRQ()   			__set_PRIMASK(0);

#define UART_BAUD								(115200)

#ifdef 	ENABLE_DEBUG_LOG
#define DebugLog								printf
#else
#define DebugLog(...)
#endif

#define UART_NBUF								(236)
#define RF_NBUF									UART_NBUF
#define UART_SOF								(0x5C)							//帧头
#define UART_EOF 								(0xCA)							//帧尾
#define UartOK	 								(0)									//串口接收帧完成
#define UartHEADER 							(1)									//串口接收帧帧头
#define UartTYPE 								(2)									//串口接收帧数据
#define UartLEN									(3)									//串口接收帧异或
#define UartSIGN                (4)
#define UartDATA 								(5)									//串口接收帧帧尾
#define UartXOR									(6)									//串口接收帧异或
#define UartEND 								(7)									//串口接收帧帧尾


#define TX_ADR_WIDTH 						(5)
#define RX_ADR_WIDTH 						(5)
#define RF_CHANNEL_NRF1					(30)
#define RF_CHANNEL_NRF2					(8)

//#define TX_MODE

#endif //_DEFINE_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/
