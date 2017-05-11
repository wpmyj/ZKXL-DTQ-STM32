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
#define NRF2_SPI_MISO_PIN             GPIO_Pin_14
#define NRF2_SPI_MOSI_PORT            GPIOB
#define NRF2_SPI_MOSI_CLK             RCC_APB2Periph_GPIOB
#define NRF2_SPI_MOSI_PIN             GPIO_Pin_15
#define NRF2_SPI_SCK_PORT             GPIOB
#define NRF2_SPI_SCK_CLK              RCC_APB2Periph_GPIOB
#define NRF2_SPI_SCK_PIN              GPIO_Pin_13
#define NRF2_SPI_CSN_PORT             GPIOB
#define NRF2_SPI_CSN_CLK              RCC_APB2Periph_GPIOB
#define NRF2_SPI_CSN_PIN              GPIO_Pin_3
#define NRF2_SPI_CE_PORT              GPIOB
#define NRF2_SPI_CE_CLK               RCC_APB2Periph_GPIOB
#define NRF2_SPI_CE_PIN               GPIO_Pin_4
#define NRF2_SPI_IRQ_PORT             GPIOB
#define NRF2_SPI_IRQ_CLK              RCC_APB2Periph_GPIOB
#define NRF2_SPI_IRQ_PIN              GPIO_Pin_5
#define NRF2_EXTI_LINE_RFIRQ          EXTI_Line5
#define NRF2_RFIRQ_PinSource          GPIO_PinSource5
#define NRF2_RFIRQ_PortSource         GPIO_PortSourceGPIOB
#define NRF2_RFIRQ_EXTI_IRQn          EXTI9_5_IRQn
#define NRF2_RFIRQ_EXTI_IRQHandler	  EXTI9_5_IRQHandler

#define NRF1_CSN_LOW()                NRF1_SPI_CSN_PORT->BRR  = NRF1_SPI_CSN_PIN
#define NRF1_CSN_HIGH()               NRF1_SPI_CSN_PORT->BSRR = NRF1_SPI_CSN_PIN
#define NRF1_CE_LOW()                 NRF1_SPI_CE_PORT->BRR   = NRF1_SPI_CE_PIN
#define NRF1_CE_HIGH()                NRF1_SPI_CE_PORT->BSRR  = NRF1_SPI_CE_PIN

#define NRF2_CSN_LOW()                NRF2_SPI_CSN_PORT->BRR  = NRF2_SPI_CSN_PIN
#define NRF2_CSN_HIGH()               NRF2_SPI_CSN_PORT->BSRR = NRF2_SPI_CSN_PIN
#define NRF2_CE_LOW()                 NRF2_SPI_CE_PORT->BRR   = NRF2_SPI_CE_PIN
#define NRF2_CE_HIGH()                NRF2_SPI_CE_PORT->BSRR  = NRF2_SPI_CE_PIN


/* MFRC500 configuration defines ---------------------------------------------*/
#define MFRC500_PD_Port                GPIOC
#define	MFRC500_PD_Pin                 GPIO_Pin_6
#define MFRC500_IRQ_Port               GPIOC
#define	MFRC500_IRQ_Pin                GPIO_Pin_7
#define MFRC500_ALE_Port               GPIOC
#define	MFRC500_ALE_Pin                GPIO_Pin_8
#define MFRC500_CS_Port                GPIOC
#define	MFRC500_CS_Pin                 GPIO_Pin_9
#define MFRC500_WR_Port                GPIOA
#define	MFRC500_WR_Pin                 GPIO_Pin_8
#define MFRC500_RD_Port                GPIOA
#define	MFRC500_RD_Pin                 GPIO_Pin_15
#define MFRC500_DATA_Port              GPIOB

#define MFRC500_RD_PORT                GPIOA
#define MFRC500_WR_PORT                GPIOA
#define MFRC500_CTL_PORT               GPIOC

#define ALE(n)                         GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_ALE_Pin, n)
#define	CS(n)                          GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_CS_Pin , n)
#define	RD(n)                          GPIO_WriteBit(MFRC500_RD_PORT , MFRC500_RD_Pin , n)
#define	WR(n)                          GPIO_WriteBit(MFRC500_WR_PORT , MFRC500_WR_Pin , n)
#define	PD(n)                          GPIO_WriteBit(MFRC500_CTL_PORT, MFRC500_PD_Pin , n)
#define	WR_DATA(n) 			               { GPIOB->ODR &= 0xFFFFE0FF;GPIOB->ODR |= (n &0x1F)<<8;   \
                                         GPIOC->ODR &= 0xFFFFE3FF;GPIOC->ODR |= (n &0xE0)<<5; }
#define RD_DATA()				               (uint8_t)((((GPIOB->IDR) & 0x00001F00)>>8) |             \
																       (((GPIOC->IDR)&0x00001C00) >> 10)<<5)
#define MFRC500_DATA_IN()              {	GPIOB->CRH &= 0xFFF00000;GPIOB->CRH |= 0x00044444;    \
																	        GPIOC->CRH &= 0xFFF000FF;GPIOC->CRH |= 0x00044400;}
#define MFRC500_DATA_OUT()             { GPIOB->CRH &= 0xFFF00000;GPIOB->CRH |= 0x00033333;     \
	                                       GPIOC->CRH &= 0xFFF000FF;GPIOC->CRH |= 0x00033300;}

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
