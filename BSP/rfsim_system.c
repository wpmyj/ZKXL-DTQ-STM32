/**
  ******************************************************************************
  * @file   	rfsim_system.c
  * @author  	Tim.Xia
  * @version 	V1.0.0
  * @date   	1-6-2013
  * @brief   
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rfsim_system.h"

/* Exported Function ---------------------------------------------------------*/
/* Exported Function ---------------------------------------------------------*/

/*******************************************************************************
  * @brief  The Whole Board Init.
  * @param  None
  * @retval None
*******************************************************************************/
void RCC_Configuration(void)
{
	SystemInit();
	// systickInit();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

/* Delay Exported Function ---------------------------------------------------*/
/* Delay Exported Function ---------------------------------------------------*/

/*******************************************************************************
  * @brief  Initialize SysTick
  * @param  None
  * @retval None		 
*******************************************************************************/
void systickInit(void)
{
	if(SysTick_Config(SystemCoreClock / 1000))
		while(1);
}

volatile uint32_t GulDelayCount = 0;				/* SysTick Counter.Global VARIABLE		*/
/*******************************************************************************
  * @brief  Delay few miliseconds.
  * @param  NMS: numbers of your want delay ms.
  * @retval None
*******************************************************************************/
void delay10Us(uint32_t ulNms)
{
	GulDelayCount = ulNms;
	while(GulDelayCount);
}

/*******************************************************************************
  * @brief  ISR Handler of SysTick  
  * @param  None
  * @retval None
*******************************************************************************/
void systDelayIntHandler(void)
{
	if(GulDelayCount-- != 0x00)
	{
	}
}


/*******************END OF FILE************************************************/









