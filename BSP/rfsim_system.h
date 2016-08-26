/**
  ******************************************************************************
  * @file    	rfsim_system.h
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	1-4-2013
  * @brief   
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RESIM_SYSTEM_H__
#define __RESIM_SYSTEM_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void RCC_Configuration(void);
void systickInit(void);
void systDelayIntHandler(void);
void delay10Us(uint32_t ulNms);


#endif

/*******************END OF FILE***********************************************/

