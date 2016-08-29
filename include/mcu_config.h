/**
  ******************************************************************************
  * @file   	mcu_config.h
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	pin and port config
  ******************************************************************************
  */

#ifndef _MCU_CONFIG_H_
#define _MCU_CONFIG_H_
/* NVIC defines ---------------------------------------------------------------*/
/* set system nvic group */
#define SYSTEM_MVIC_GROUP_SET         NVIC_PriorityGroup_2

/* Peripheral interrupt preemption set */
#define UART1_PREEMPTION_PRIORITY     1
#define UART2_PREEMPTION_PRIORITY     1
#define NRF_PREEMPTION_PRIORITY       1 

/* Peripheral interrupt response set */
#define UART1_SUB_PRIORITY            1
#define UART2_SUB_PRIORITY            1
#define NRF_SUB_PRIORITY              0 

/* USART defines -------------------------------------------------------------*/
/* USART defines -------------------------------------------------------------*/
#define USART1pos              			USART1
#define USART1pos_GPIO         			GPIOA
#define USART1pos_CLK          			RCC_APB2Periph_USART1
#define USART1pos_GPIO_CLK     			RCC_APB2Periph_GPIOA
#define USART1pos_RxPin        			GPIO_Pin_10
#define USART1pos_TxPin        			GPIO_Pin_9
#define USART1pos_IRQn         			USART1_IRQn
#define USART1pos_IRQHandler   			USART1_IRQHandler

#define USART2pos              			USART2
#define USART2pos_GPIO         			GPIOA
#define USART2pos_CLK          			RCC_APB1Periph_USART2
#define USART2pos_GPIO_CLK     			RCC_APB2Periph_GPIOA
#define USART2pos_RxPin        			GPIO_Pin_3
#define USART2pos_TxPin        			GPIO_Pin_2
#define USART2pos_IRQn         			USART2_IRQn
#define USART2pos_IRQHandler   			USART2_IRQHandler

/* SPI defines ---------------------------------------------------------------*/

#define SE2431L_CTX_PORT				GPIOC	
#define SE2431L_CTX_CLK					RCC_APB2Periph_GPIOC
#define SE2431L_CTX_PIN					GPIO_Pin_12
#define SE2431L_CPS_PORT				GPIOB
#define SE2431L_CPS_CLK					RCC_APB2Periph_GPIOB
#define SE2431L_CPS_PIN					GPIO_Pin_0
#define SE2431L_CSD_PORT				GPIOA
#define SE2431L_CSD_CLK					RCC_APB2Periph_GPIOA
#define SE2431L_CSD_PIN					GPIO_Pin_11

/*****************************************************************************
*端口定义
*****************************************************************************/
#define  MFRC500_PD_Port		GPIOC
#define	 MFRC500_PD_Pin 		GPIO_Pin_6

#define  MFRC500_IRQ_Port		GPIOC
#define	 MFRC500_IRQ_Pin 		GPIO_Pin_7

#define  MFRC500_ALE_Port		GPIOC
#define	 MFRC500_ALE_Pin 		GPIO_Pin_8

#define  MFRC500_CS_Port		GPIOC
#define	 MFRC500_CS_Pin 		GPIO_Pin_9

#define  MFRC500_WR_Port		GPIOA
#define	 MFRC500_WR_Pin 		GPIO_Pin_8

#define  MFRC500_RD_Port		GPIOA
#define	 MFRC500_RD_Pin 		GPIO_Pin_15

#define  MFRC500_DATA_Port		GPIOB
#define	 MFRC500_DATA_Pin 		(\
								GPIO_Pin_8|\
								GPIO_Pin_9|\
								GPIO_Pin_10|\
								GPIO_Pin_11|\
								GPIO_Pin_12|\
								GPIO_Pin_13|\
								GPIO_Pin_14|\
								GPIO_Pin_15\
								)

#define  MFRC500_Control_Port	GPIOC
#define  MFRC500_RDWR_Port		GPIOA

/*****************************************************************************
*端口操作
*****************************************************************************/
#define ALE(n)					GPIO_WriteBit(MFRC500_Control_Port, MFRC500_ALE_Pin, n)
#define	CS(n)					GPIO_WriteBit(MFRC500_Control_Port, MFRC500_CS_Pin, n)
#define	RD(n)					GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_RD_Pin, n)
#define	WR(n)					GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_WR_Pin, n)
#define	PD(n)					GPIO_WriteBit(MFRC500_Control_Port, MFRC500_PD_Pin, n)
#define	WR_DATA(n) 				GPIO_Write(MFRC500_DATA_Port, (uint16_t)(n))
#define RD_DATA()				(uint8_t)(GPIO_ReadInputData(MFRC500_DATA_Port) >> 8)

#endif //_MCU_CONFIG_H_
/**
  * @}
  */
/**************************************END OF FILE****************************/
