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
/* NVIC defines ---------------------------------------------------------------*/
/* set system nvic group */
#define SYSTEM_MVIC_GROUP_SET         NVIC_PriorityGroup_2

/* Peripheral interrupt preemption set */
#define UART1_PREEMPTION_PRIORITY     1
#define UART2_PREEMPTION_PRIORITY     1
#define NRF1_PREEMPTION_PRIORITY      0
#define NRF2_PREEMPTION_PRIORITY      1
#define TIM3_PREEMPTION_PRIORITY      0

/* Peripheral interrupt response set */
#define UART1_SUB_PRIORITY            1
#define UART2_SUB_PRIORITY            1
#define NRF_SUB_PRIORITY              0 
#define TIM3_SUB_PRIORITY             1

/* USART defines -------------------------------------------------------------*/
/* USART defines -------------------------------------------------------------*/
#define USART1pos                     USART1
#define USART1pos_GPIO                GPIOA
#define USART1pos_CLK                 RCC_APB2Periph_USART1
#define USART1pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART1pos_RxPin               GPIO_Pin_10
#define USART1pos_TxPin               GPIO_Pin_9
#define USART1pos_IRQn                USART1_IRQn
#define USART1pos_IRQHandler          USART1_IRQHandler

#define USART2pos                     USART2
#define USART2pos_GPIO                GPIOA
#define USART2pos_CLK                 RCC_APB1Periph_USART2
#define USART2pos_GPIO_CLK            RCC_APB2Periph_GPIOA
#define USART2pos_RxPin               GPIO_Pin_3
#define USART2pos_TxPin               GPIO_Pin_2
#define USART2pos_IRQn                USART2_IRQn
#define USART2pos_IRQHandler          USART2_IRQHandler

//#define ZL_RP551_MAIN_E
#define ZL_RP551_MAIN_F

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
