/**
  ******************************************************************************
  * @file   	clock.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	clock config functions and delay functions
  ******************************************************************************
  */
  
#include "main.h"

//uint8_t 		      NDEF_DataWrite[30] = {0x00, 0x00, 0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00};
//uint8_t 					NDEF_DataRead[0xFF] = {0x00};
//uint16_t				  NDEF_Len = 0;

bool 						  gbf_hse_setup_fail = FALSE;		// 外部16M晶振起振标志
//uint8_t 					time_for_buzzer_on = 0;			  // 多长时间之后开始响
//uint16_t 					time_for_buzzer_off = 0;		  // 响了多长时间之后关

RF_TypeDef 				rf_var;							          // 2.4G数据包缓冲

uint16_t					delay_nms = 0;					      // 中断延时变量

//uint8_t 					g_cardType[40] = {0x00};		    // 返回卡类型
//uint8_t 					respon[BUF_LEN + 20] = {0x00};		

/**************************************END OF FILE****************************/
