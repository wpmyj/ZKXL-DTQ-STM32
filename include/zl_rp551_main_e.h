/**
******************************************************************************
* @file       zl_rp551_main_e.h
* @author      Samwu
* @version     V1.0.0.0
* @date       2017.01.12
* @brief       board infomation
******************************************************************************
*/

#ifndef _ZL_RP551_MAIN_E_H_
#define _ZL_RP551_MAIN_E_H_
/* board name defines --------------------------------------------------------*/
/* NVIC defines ---------------------------------------------------------------*/
/* set system nvic group */
#define SYSTEM_MVIC_GROUP_SET         NVIC_PriorityGroup_2

/* Peripheral interrupt preemption set */
#define UART1_PREEMPTION_PRIORITY     1
#define UART2_PREEMPTION_PRIORITY     1
#define NRF_PREEMPTION_PRIORITY       0
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

/* LNA defines ---------------------------------------------------------------*/
#define SE2431L_CTX_PORT              GPIOC    
#define SE2431L_CTX_CLK               RCC_APB2Periph_GPIOC
#define SE2431L_CTX_PIN               GPIO_Pin_12
#define SE2431L_CPS_PORT              GPIOB
#define SE2431L_CPS_CLK               RCC_APB2Periph_GPIOB
#define SE2431L_CPS_PIN               GPIO_Pin_0
#define SE2431L_CSD_PORT              GPIOA
#define SE2431L_CSD_CLK               RCC_APB2Periph_GPIOA
#define SE2431L_CSD_PIN               GPIO_Pin_11

/* SPI configuration defines -------------------------------------------------*/
#define SPI_MISO_PORT                  GPIOA
#define SPI_MISO_CLK                   RCC_APB2Periph_GPIOA
#define SPI_MISO_PIN                   GPIO_Pin_6

#define SPI_MOSI_PORT                  GPIOA
#define SPI_MOSI_CLK                   RCC_APB2Periph_GPIOA
#define SPI_MOSI_PIN                   GPIO_Pin_7

#define SPI_SCK_PORT                   GPIOA
#define SPI_SCK_CLK                    RCC_APB2Periph_GPIOA
#define SPI_SCK_PIN                    GPIO_Pin_5

#define SPI_CSN_PORT                   GPIOA
#define SPI_CSN_CLK                    RCC_APB2Periph_GPIOA
#define SPI_CSN_PIN                    GPIO_Pin_4

#define SPI_CE_PORT                    GPIOC
#define SPI_CE_CLK                     RCC_APB2Periph_GPIOC
#define SPI_CE_PIN                     GPIO_Pin_4

#define SPI_IRQ_PORT                   GPIOC
#define SPI_IRQ_CLK                    RCC_APB2Periph_GPIOC
#define SPI_IRQ_PIN                    GPIO_Pin_5

#define EXTI_LINE_RFIRQ                EXTI_Line5
#define RFIRQ_PinSource                GPIO_PinSource5
#define RFIRQ_PortSource               GPIO_PortSourceGPIOC
#define RFIRQ_EXTI_IRQn                EXTI9_5_IRQn
#define RFIRQ_EXTI_IRQHandler          EXTI9_5_IRQHandler

#define SPI_CSN_PORT_2                 GPIOB
#define SPI_CSN_CLK_2                  RCC_APB2Periph_GPIOB
#define SPI_CSN_PIN_2                  GPIO_Pin_3

#define SPI_CE_PORT_2                  GPIOB
#define SPI_CE_CLK_2                   RCC_APB2Periph_GPIOB
#define SPI_CE_PIN_2                   GPIO_Pin_5

#define SPI_IRQ_PORT_2                 GPIOB
#define SPI_IRQ_CLK_2                  RCC_APB2Periph_GPIOB
#define SPI_IRQ_PIN_2                  GPIO_Pin_3

#define EXTI_LINE_RFIRQ_2              EXTI_Line3
#define RFIRQ_PinSource_2              GPIO_PinSource3
#define RFIRQ_PortSource_2             GPIO_PortSourceGPIOB
#define RFIRQ_EXTI_IRQn_2              EXTI3_IRQn
#define RFIRQ_EXTI_IRQHandler_2        EXTI3_IRQHandler

#define SPI_CSN_HIGH()                 GPIO_SetBits(SPI_CSN_PORT, SPI_CSN_PIN)
#define SPI_CSN_LOW()                  GPIO_ResetBits(SPI_CSN_PORT, SPI_CSN_PIN)
#define SPI_CSN_HIGH_2()               GPIO_SetBits(SPI_CSN_PORT_2, SPI_CSN_PIN_2)
#define SPI_CSN_LOW_2()                GPIO_ResetBits(SPI_CSN_PORT_2, SPI_CSN_PIN_2)

#define CSN_LOW()                      GPIOA->BRR  = GPIO_Pin_4
#define CSN_HIGH()                     GPIOA->BSRR = GPIO_Pin_4
#define CE_LOW()                       GPIOC->BRR  = GPIO_Pin_4
#define CE_HIGH()                      GPIOC->BSRR = GPIO_Pin_4

#define CSN_LOW_2()                    GPIOB->BRR  = GPIO_Pin_3
#define CSN_HIGH_2()                   GPIOB->BSRR = GPIO_Pin_3
#define CE_LOW_2()                     GPIOC->BRR  = GPIO_Pin_4
#define CE_HIGH_2()                    GPIOC->BSRR = GPIO_Pin_4

/* MFRC500 configuration defines ---------------------------------------------*/
#define MFRC500_PD_Port                GPIOC
#define MFRC500_PD_Pin                 GPIO_Pin_6
#define MFRC500_IRQ_Port               GPIOC
#define MFRC500_IRQ_Pin                GPIO_Pin_7
#define MFRC500_ALE_Port               GPIOC
#define MFRC500_ALE_Pin                GPIO_Pin_8
#define MFRC500_CS_Port                GPIOC
#define MFRC500_CS_Pin                 GPIO_Pin_9
#define MFRC500_WR_Port                GPIOA
#define MFRC500_WR_Pin                 GPIO_Pin_8
#define MFRC500_RD_Port                GPIOA
#define MFRC500_RD_Pin                 GPIO_Pin_15
#define MFRC500_DATA_Port              GPIOB
#define MFRC500_DATA_Pin              (GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10|\
                                       GPIO_Pin_11| GPIO_Pin_12| GPIO_Pin_13|\
                                       GPIO_Pin_14| GPIO_Pin_15 )
#define  MFRC500_Control_Port          GPIOC
#define  MFRC500_RDWR_Port             GPIOA

#define ALE(n)                         GPIO_WriteBit(MFRC500_Control_Port, MFRC500_ALE_Pin, n)
#define CS(n)                          GPIO_WriteBit(MFRC500_Control_Port, MFRC500_CS_Pin, n)
#define RD(n)                          GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_RD_Pin, n)
#define WR(n)                          GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_WR_Pin, n)
#define PD(n)                          GPIO_WriteBit(MFRC500_Control_Port, MFRC500_PD_Pin, n)
#define WR_DATA(n)                     GPIO_Write(MFRC500_DATA_Port, (uint16_t)(n))
#define RD_DATA()                     (uint8_t)(GPIO_ReadInputData(MFRC500_DATA_Port) >> 8)

/* LED defines ---------------------------------------------------------------*/
#define LEDn                           4
#define LED1_GPIO_PORT                 GPIOC
#define LED1_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define LED1_GPIO_PIN                  GPIO_Pin_1
#define LED2_GPIO_PORT                 GPIOC
#define LED2_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define LED2_GPIO_PIN                  GPIO_Pin_2
#define LGREEN_GPIO_PORT               GPIOB
#define LGREEN_GPIO_CLK                RCC_APB2Periph_GPIOB
#define LGREEN_GPIO_PIN                GPIO_Pin_6
#define LBLUE_GPIO_PORT                GPIOB
#define LBLUE_GPIO_CLK                 RCC_APB2Periph_GPIOB
#define LBLUE_GPIO_PIN                 GPIO_Pin_7

/* BEEP defines ---------------------------------------------------------------*/
#define BEEP_PORT                      GPIOC
#define BEEP_CLK                       RCC_APB2Periph_GPIOC
#define BEEP_PIN                       GPIO_Pin_0
#define BEEP_EN()                     {GPIO_SetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_DISEN()                  {GPIO_ResetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TOGGLE()                 {BEEP_PORT->ODR ^ = BEEP_PIN;}

#endif //_ZL_RP551_MAIN_E_H_
/**************************************END OF FILE****************************/
