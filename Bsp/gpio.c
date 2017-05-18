/**
  ******************************************************************************
  * @file   	led.c
  * @author  	sam.wu
  * @version 	V1.0.0.0
  * @date   	2016.8.26
  * @brief   	led driver
  ******************************************************************************
  */
#include "gpio.h"

/* LED Variables -------------------------------------------------------------*/
GPIO_TypeDef *GPIO_PORT[LEDn] = {
	LED1_GPIO_PORT,  
	LED2_GPIO_PORT, 
	LRED_GPIO_PORT, 
	LBLUE_GPIO_PORT
};

const uint16_t GPIO_PIN[LEDn] = {
	LED1_GPIO_PIN, 
	LED2_GPIO_PIN, 
	LRED_GPIO_PIN, 
	LBLUE_GPIO_PIN
};

const uint32_t GPIO_CLK[LEDn] = {
	LED1_GPIO_CLK, 
	LED2_GPIO_CLK, 
	LRED_GPIO_CLK, 
	LBLUE_GPIO_CLK
};

/* LED Functions ------------------------------------------------------------ */
/*******************************************************************************
  * @brief  Configures LED GPIO.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
*******************************************************************************/
void ledInit(Led_TypeDef Led)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO_LED Clock */
	RCC_APB2PeriphClockCmd(GPIO_CLK[Led], ENABLE);

	/* Configure the GPIO_LED pin */
	GPIO_InitStructure.GPIO_Pin = GPIO_PIN[Led];
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIO_PORT[Led], &GPIO_InitStructure);
}

/*******************************************************************************
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
*******************************************************************************/
void ledOn(Led_TypeDef Led)
{
	if((Led == LED1) || (Led == LED2))
		GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
	else if((Led == LRED) || (Led == LBLUE))
		GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
}

/*******************************************************************************
  * @brief  Turns selected LED Off.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
*******************************************************************************/
void ledOff(Led_TypeDef Led)
{
	if((Led == LED1) || (Led == LED2))
		GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
	else if((Led == LRED) || (Led == LBLUE))
		GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
}

/*******************************************************************************
  * @brief  Toggles the selected LED.
  * @param  Led: Specifies the Led to be set off.
  *   This parameter can be one of following parameters:
  *     @arg LED1
  *     @arg LED2
  * @retval None
*******************************************************************************/
void ledToggle(Led_TypeDef Led)
{
	GPIO_PORT[Led]->ODR ^= GPIO_PIN[Led];
}

/*******************************************************************************
  * @brief  Configures beep GPIO.
  * @param  None
  * @retval None
*******************************************************************************/
void GPIOInit_BEEP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);	
}

/*******************************************************************************
  * @brief  Initialize the Gpio port for system
  * @param  None
  * @retval None
*******************************************************************************/
void bsp_board_init(void)
{
	/* Initialize Led function */
	ledInit(LRED);
	ledInit(LBLUE);
	
	/* Initialize beep function */
	GPIOInit_BEEP();
}

/************************END OF FILE******************************************/
