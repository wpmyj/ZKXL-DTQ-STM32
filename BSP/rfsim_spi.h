/**
  ******************************************************************************
  * @file   	rfsim_spi.h
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	24-5-2013
  * @brief   	
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFSIM_SPI_H__
#define __RFSIM_SPI_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Exported types ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

  
/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/

#define SPI_MISO_PORT              	GPIOB
#define SPI_MISO_CLK               	RCC_APB2Periph_GPIOB
#define SPI_MISO_PIN               	GPIO_Pin_14

#define SPI_MOSI_PORT              	GPIOB
#define SPI_MOSI_CLK               	RCC_APB2Periph_GPIOB
#define SPI_MOSI_PIN               	GPIO_Pin_15

#define SPI_SCK_PORT              	GPIOB
#define SPI_SCK_CLK               	RCC_APB2Periph_GPIOB
#define SPI_SCK_PIN               	GPIO_Pin_13

#define SPI_CSN_PORT              	GPIOB
#define SPI_CSN_CLK               	RCC_APB2Periph_GPIOB
#define SPI_CSN_PIN               	GPIO_Pin_12

#define SPI_CE_PORT              	GPIOC
#define SPI_CE_CLK               	RCC_APB2Periph_GPIOC
#define SPI_CE_PIN               	GPIO_Pin_6

#define SPI_IRQ_PORT              	GPIOC
#define SPI_IRQ_CLK               	RCC_APB2Periph_GPIOC
#define SPI_IRQ_PIN               	GPIO_Pin_7


/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */

void spiInit(void);


#endif

/*******************END OF FILE***********************************************/









