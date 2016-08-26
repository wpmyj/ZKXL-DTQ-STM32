/**
  ******************************************************************************
  * @file   	RFSIM_Usart.h
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	21-4-2013
  * @brief   	
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFSIM_USART_H__
#define __RFSIM_USART_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

  
/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

#define EVAL_COM1                   USART1
#define EVAL_COM1_GPIO              GPIOA
#define EVAL_COM1_CLK               RCC_APB2Periph_USART1
#define EVAL_COM1_GPIO_CLK          RCC_APB2Periph_GPIOA
#define EVAL_COM1_RxPin             GPIO_Pin_10
#define EVAL_COM1_TxPin             GPIO_Pin_9


/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */

void uartInit(void);
void fputc(uint8_t *puBuf, uint8_t uWidth);


#endif

/*******************END OF FILE***********************************************/









