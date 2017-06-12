/**
  ******************************************************************************
  * @file   	zl_rp551_main_h.h
  * @author  	Samwu
  * @version 	V1.0.0.0
  * @date   	2017.01.12
  * @brief   	board infomation
  ******************************************************************************
  */

#ifndef _ZL_RP551_MAIN_H_H_
#define _ZL_RP551_MAIN_H_H_
/* SPI configuration defines -------------------------------------------------*/
/* RX:NRF1 IO Map */
#define NRF1_SPI_MISO_PORT            GPIOA
#define NRF1_SPI_MISO_CLK             RCC_APB2Periph_GPIOA
#define NRF1_SPI_MISO_PIN             GPIO_Pin_6
#define NRF1_SPI_MOSI_PORT            GPIOA
#define NRF1_SPI_MOSI_CLK             RCC_APB2Periph_GPIOA
#define NRF1_SPI_MOSI_PIN             GPIO_Pin_7
#define NRF1_SPI_SCK_PORT             GPIOA
#define NRF1_SPI_SCK_CLK              RCC_APB2Periph_GPIOA
#define NRF1_SPI_SCK_PIN              GPIO_Pin_5
#define NRF1_SPI_CSN_PORT             GPIOA
#define NRF1_SPI_CSN_CLK              RCC_APB2Periph_GPIOA
#define NRF1_SPI_CSN_PIN              GPIO_Pin_4
#define NRF1_SPI_CE_PORT              GPIOC
#define NRF1_SPI_CE_CLK               RCC_APB2Periph_GPIOC
#define NRF1_SPI_CE_PIN               GPIO_Pin_4
#define NRF1_SPI_IRQ_PORT             GPIOC
#define NRF1_SPI_IRQ_CLK              RCC_APB2Periph_GPIOC
#define NRF1_SPI_IRQ_PIN              GPIO_Pin_5
#define NRF1_EXTI_LINE_RFIRQ          EXTI_Line5
#define NRF1_RFIRQ_PinSource          GPIO_PinSource5
#define NRF1_RFIRQ_PortSource         GPIO_PortSourceGPIOC
#define NRF1_RFIRQ_EXTI_IRQn          EXTI9_5_IRQn
#define NRF1_RFIRQ_EXTI_IRQHandler    EXTI9_5_IRQHandler

/* TX:NRF2 IO Map */
#define NRF2_SPI_MISO_PORT            GPIOB
#define NRF2_SPI_MISO_CLK             RCC_APB2Periph_GPIOB
#define NRF2_SPI_MISO_PIN             GPIO_Pin_4
#define NRF2_SPI_MOSI_PORT            GPIOB
#define NRF2_SPI_MOSI_CLK             RCC_APB2Periph_GPIOB
#define NRF2_SPI_MOSI_PIN             GPIO_Pin_5
#define NRF2_SPI_SCK_PORT             GPIOB
#define NRF2_SPI_SCK_CLK              RCC_APB2Periph_GPIOB
#define NRF2_SPI_SCK_PIN              GPIO_Pin_3
#define NRF2_SPI_CSN_PORT             GPIOC
#define NRF2_SPI_CSN_CLK              RCC_APB2Periph_GPIOC
#define NRF2_SPI_CSN_PIN              GPIO_Pin_11
#define NRF2_SPI_CE_PORT              GPIOC
#define NRF2_SPI_CE_CLK               RCC_APB2Periph_GPIOC
#define NRF2_SPI_CE_PIN               GPIO_Pin_12
#define NRF2_SPI_IRQ_PORT             GPIOD
#define NRF2_SPI_IRQ_CLK              RCC_APB2Periph_GPIOD
#define NRF2_SPI_IRQ_PIN              GPIO_Pin_2
#define NRF2_EXTI_LINE_RFIRQ          EXTI_Line5
#define NRF2_RFIRQ_PinSource          GPIO_PinSource2
#define NRF2_RFIRQ_PortSource         GPIO_PortSourceGPIOD
#define NRF2_RFIRQ_EXTI_IRQn          EXTI2_IRQn
#define NRF2_RFIRQ_EXTI_IRQHandler    EXTI2_IRQHandler


/* 13.56Mhz:MF IO Map */
#define MF_SPI_MISO_PORT              GPIOB
#define MF_SPI_MISO_CLK               RCC_APB2Periph_GPIOB
#define MF_SPI_MISO_PIN               GPIO_Pin_14
#define MF_SPI_MOSI_PORT              GPIOB
#define MF_SPI_MOSI_CLK               RCC_APB2Periph_GPIOB
#define MF_SPI_MOSI_PIN               GPIO_Pin_15
#define MF_SPI_SCK_PORT               GPIOB
#define MF_SPI_SCK_CLK                RCC_APB2Periph_GPIOB
#define MF_SPI_SCK_PIN                GPIO_Pin_13
#define MF_SPI_CSN_PORT               GPIOB
#define MF_SPI_CSN_CLK                RCC_APB2Periph_GPIOB
#define MF_SPI_CSN_PIN                GPIO_Pin_12

#define MF_RSTPD_PORT                 GPIOC
#define MF_RSTPD_CLK                  RCC_APB2Periph_GPIOC
#define MF_RSTPD_PIN                  GPIO_Pin_6
#define MF_IRQ_PORT                   GPIOC
#define MF_IRQ_CLK                    RCC_APB2Periph_GPIOC
#define MF_IRQ_PIN                    GPIO_Pin_7

#define MF_EXTI_LINE_RFIRQ            EXTI_Line7
#define MF_RFIRQ_PinSource            GPIO_PinSource7
#define MF_RFIRQ_PortSource           GPIO_PortSourceGPIOC
#define MF_RFIRQ_EXTI_IRQn            EXTI9_5_IRQn
#define MF_RFIRQ_EXTI_IRQHandler	    EXTI9_5_IRQHandler

#define NRF1_CSN_LOW()                NRF1_SPI_CSN_PORT->BRR  = NRF1_SPI_CSN_PIN
#define NRF1_CSN_HIGH()               NRF1_SPI_CSN_PORT->BSRR = NRF1_SPI_CSN_PIN
#define NRF1_CE_LOW()                 NRF1_SPI_CE_PORT->BRR   = NRF1_SPI_CE_PIN
#define NRF1_CE_HIGH()                NRF1_SPI_CE_PORT->BSRR  = NRF1_SPI_CE_PIN

#define NRF2_CSN_LOW()                NRF2_SPI_CSN_PORT->BRR  = NRF2_SPI_CSN_PIN
#define NRF2_CSN_HIGH()               NRF2_SPI_CSN_PORT->BSRR = NRF2_SPI_CSN_PIN
#define NRF2_CE_LOW()                 NRF2_SPI_CE_PORT->BRR   = NRF2_SPI_CE_PIN
#define NRF2_CE_HIGH()                NRF2_SPI_CE_PORT->BSRR  = NRF2_SPI_CE_PIN

#define MF_CSN_LOW()                  MF_SPI_CSN_PORT->BRR  = MF_SPI_CSN_PIN
#define MF_CSN_HIGH()                 MF_SPI_CSN_PORT->BSRR = MF_SPI_CSN_PIN
#define	CS(n)                         GPIO_WriteBit(MF_SPI_CSN_PORT, MF_SPI_CSN_PIN , n)

#define MF_RSTPD_LOW()                MF_RSTPD_PORT->BRR   = MF_RSTPD_PIN
#define MF_RSTPD_HIGH()               MF_RSTPD_PORT->BSRR  = MF_RSTPD_PIN
#define	PD(n)                         GPIO_WriteBit(MF_RSTPD_PORT, MF_RSTPD_PIN , n)

/* LED defines ---------------------------------------------------------------*/
#define LEDn                           4
#define LED1_GPIO_PORT                 GPIOC
#define LED1_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define LED1_GPIO_PIN                  GPIO_Pin_1
#define LED2_GPIO_PORT                 GPIOC
#define LED2_GPIO_CLK                  RCC_APB2Periph_GPIOC
#define LED2_GPIO_PIN                  GPIO_Pin_2
#define LRED_GPIO_PORT                 GPIOA
#define LRED_GPIO_CLK                  RCC_APB2Periph_GPIOA
#define LRED_GPIO_PIN                  GPIO_Pin_1
#define LBLUE_GPIO_PORT                GPIOA
#define LBLUE_GPIO_CLK                 RCC_APB2Periph_GPIOA
#define LBLUE_GPIO_PIN                 GPIO_Pin_0

/* BEEP defines ---------------------------------------------------------------*/
#define BEEP_PORT                      GPIOC
#define BEEP_CLK                       RCC_APB2Periph_GPIOC
#define BEEP_PIN                       GPIO_Pin_0
#define BEEP_EN()                     {GPIO_SetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_DISEN()                  {GPIO_ResetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TOGGLE()                 {BEEP_PORT->ODR ^ = BEEP_PIN;}


#endif //_ZL_RP551_MAIN_F_H_

/**************************************END OF FILE****************************/
