/**
  ******************************************************************************
  * @file   	board.h
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board infomation
  ******************************************************************************
  */

#ifndef _BOARD_H_
#define _BOARD_H_
/* board name defines --------------------------------------------------------*/

#define ZL_RP551_MAIN_E
//#define ZL_RP551_MAIN_F

#if !defined (ZL_RP551_MAIN_E) && !defined (ZL_RP551_MAIN_F)
 #error "Please select board used in your application (in board.h file)"
#endif

#if defined (ZL_RP551_MAIN_E) && defined (ZL_RP551_MAIN_F)
 #error "Please select only one board used in your application (in board.h file)"
#endif

#ifdef ZL_RP551_MAIN_E
#include "zl_rp551_main_e.h"
#endif

#ifdef ZL_RP551_MAIN_F
#include "zl_rp551_main_f.h"
#endif

#endif //_BOARD_H_

/**************************************END OF FILE****************************/
