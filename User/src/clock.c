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

/*******************************************************************************
  * @brief  The Whole Board Init.
  * @param  None
  * @retval None
*******************************************************************************/
void RCC_Configuration(void)
{
	SystemInit();

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_ClockSecuritySystemCmd(ENABLE);
}

#ifdef ENABLE_WATCHDOG	
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);								/* 写入0x5555,用于允许狗狗寄存器写入功能 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);										/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(312);														/* 喂狗时间 2s/6.4MS=312 .注意不能大于0xfff*/
	IWDG_ReloadCounter();														/* 喂狗*/
	IWDG_Enable();																/* 使能*/
}
#endif //ENABLE_WATCHDOG

void Delay2us(uint32_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 12;j++)
		{
			__nop();
		}	
	}
}

void Delay3us(void)
{
	uint16_t i;
	for(i = 0;i < 17;i++)
	{
		__nop();
	}
}

void Delay10us(uint16_t times)
{
	uint16_t i,j;
	for(i = 0; i < times; i++)
	{
		for(j = 0;j < 70;j++)
		{
			__nop();
		}		
	}	
}

void Delay1Ms(void)
{
	volatile uint16_t i = 10000;
	while (i--);
}

void Delay10Ms(void)
{
	volatile uint8_t vucCnt = 20;
	while(vucCnt--)
	{
		Delay1Ms();
	}
}

void Delay100Ms(void)
{
	volatile uint8_t vucCnt = 200;
	while(vucCnt--)
	{
		Delay1Ms();
	}
}

void Delay1s(void)
{
	volatile uint16_t vus_cnt = 1000;
	while (vus_cnt--)
	{
		Delay1Ms();
	}
}

/**
  * @}
  */
/**************************************END OF FILE****************************/

