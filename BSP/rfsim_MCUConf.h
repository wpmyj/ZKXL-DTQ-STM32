/**
  ******************************************************************************
  * @file   	rfsim_MCUConf.h
  * @author  	Tim.Xia
  * @version 	V2.0.0
  * @date   	31-5-2013
  * @brief    	
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __RFSIM_MCUCONF_H__
#define __RFSIM_MCUCONF_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

/* Exported typedef ----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/
/* Exported typedef ----------------------------------------------------------*/

/* LED typedef ---------------------------------------------------------------*/
/* LED typedef ---------------------------------------------------------------*/
typedef enum 
{
  LED1 = 0,
  LED2 = 1,
	LGREEN = 2,
	LBLUE = 3,
} Led_TypeDef;
	
/* Private typedefs ----------------------------------------------------------*/
/* Private typedefs ----------------------------------------------------------*/
typedef struct FLASH_PARA_STR
{
	uint8_t ucCardType;
	uint8_t ucSendMode;
	uint8_t uc8ByteOr9Byte;
	uint8_t ucOpenLnaOrNot;	
	uint8_t ucShuaiJian;
	uint8_t ucWeiKeMu;
	uint8_t ucSelfSubID;
	uint8_t ucaMasterID[4];
	uint8_t ucChangedFlag;
	uint8_t ucaOther[4];
}FLASH_PARA_T;

#define MAX_FIFO_UID 50
typedef struct WIEGAND_UID_FIFO_STR
{
	uint8_t uc2aUid[MAX_FIFO_UID][9];
	uint8_t ucTopPt;
	uint8_t ucBottomPt;
}WIEGAND_UID_FIFO_T;

#define SEND_BUF_SIZE	5100
#define COUNT_LEN 1
#define SEND_BUF_2WEI	2
typedef struct SEND_BUF_STR
{
	uint16_t usSendUidCnt;
	uint8_t  ucaSendBuf[SEND_BUF_SIZE];
}SEND_BUF_T;
	
/* FLASH PARA defines --------------------------------------------------------*/
/* FLASH PARA defines --------------------------------------------------------*/
#define CARDTYPE_CAD	0x01
#define CARDTYPE_CAI	0x02
#define CARDTYPE_CHE	0x03
#define CARDTYPE_HB20	0x04
#define CARDTYPE_HB30	0x05

#define SENDMODE_BEI	0x01 
#define SENDMODE_ZHU	0x02
#define SENDMODE_W26	0x03
#define SENDMODE_W34  0x04

#define CLOSE_LAN			0x00
#define OPEN_LNA			0x01

#define PARA_NOT_CHG	0x00
#define PARA_YES_CHG	0x01

#define UID_8BYTES		0x08
#define UID_9BYTES		0x09
#define UID_10BYTES		0x0A

#define GUANGXI_RF_TEST	0
#define DEF_PRINTF			0

/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
#define BUF2WEI 20
#define BUF2WEI_HONG 10
#define BUF1WEI 250
#define BUF3WEI 250

#define NRF1 1
#define NRF2 1

#define FLASH_LEN(ucaArray) 		(sizeof(ucaArray)/4)

#define HEARTBEAT_TIME_INTERVAL 		20
#define RF_NODATA_TIME_INTERVAL 		60
#define SOFT_RESET_TIME_INTERVAL  	21600ul
#define SOFT_RESET_NRF24_INTERVAL 	3600ul

#define RF_UID_LEN	 		 	 8
#define CHE_RF_UID_LEN		14
#define CONTINUE_10MS_CNT	 3

#define HALF_UID_CNT 			16
#define MAX_UID_CNT  			26
#define CHE_HALF_UID_CNT	 7
#define CHE_MAX_UID_CNT 	14

#define DISABLE_ALL_IRQ()  __set_PRIMASK(1);
#define ENABLE_ALL_IRQ()   __set_PRIMASK(0);

/* Time defines --------------------------------------------------------------*/
/* Time defines --------------------------------------------------------------*/
/* Base of 500us */
#define timPrescaler								(SystemCoreClock / 2000 - 1)
#define Tim2TimePiece								20

#define TIM3_timPrescaler						(SystemCoreClock / 1000000 - 1)
#define Tim3TimePiece								0xFFF0
#define WATCHDOG_DEF 		1


/* RF_PARA defines -----------------------------------------------------------*/
/* RF_PARA defines -----------------------------------------------------------*/
#define RF_CHANNEL_NRF1 8
#define RF_CHANNEL_NRF2 81
#define RF_CHANNEL_NRF3 18
#define RF_CHANNEL_NRF4 31

#define RF_CHANNEL_NRF2_ONE	81
#define RF_CHANNEL_NRF2_CHE	72

#define WDT_1S_256PRE	156
#define WDT_2S_256PRE	312

#define NVIC_PriorityGroup_Self NVIC_PriorityGroup_2

#define SET_FLAGS(uFlag)  			(uFlag = TRUE)
#define RESET_FLAGS(uFlag)  		(uFlag = FALSE)
#define RESET_COUNTER(uTmp) 		(uTmp = 0)

#define MAX_SHUAIJIAN						31
#define LNA_VAL									13
#define WHEN_MAX_FIXED(para, max_val)	(para = (para > max_val)?max_val:para)

#define SE2431L_CTX_PORT				GPIOC	
#define SE2431L_CTX_CLK					RCC_APB2Periph_GPIOC
#define SE2431L_CTX_PIN					GPIO_Pin_12
#define SE2431L_CPS_PORT				GPIOB
#define SE2431L_CPS_CLK					RCC_APB2Periph_GPIOB
#define SE2431L_CPS_PIN					GPIO_Pin_0
#define SE2431L_CSD_PORT				GPIOA
#define SE2431L_CSD_CLK					RCC_APB2Periph_GPIOA
#define SE2431L_CSD_PIN					GPIO_Pin_11
#define SKY12347_LE_PORT				GPIOC
#define SKY12347_LE_CLK					RCC_APB2Periph_GPIOC
#define SKY12347_LE_PIN					GPIO_Pin_13
#define HMC273_05db_PORT				GPIOC
#define HMC273_05db_CLK					RCC_APB2Periph_GPIOC
#define HMC273_05db_PIN					GPIO_Pin_0

/* RS485 defines -------------------------------------------------------------*/
/* RS485 defines -------------------------------------------------------------*/
#define RS485_EN_PORT								GPIOA
#define RS485_EN_CLK								RCC_APB2Periph_GPIOA
#define RS485_EN_PIN								GPIO_Pin_12
#define RS485_SEND_MODE()						{GPIO_SetBits(RS485_EN_PORT, RS485_EN_PIN);}
#define RS485_REV_MODE()						{GPIO_ResetBits(RS485_EN_PORT, RS485_EN_PIN);}

/* LED defines ---------------------------------------------------------------*/
/* LED defines ---------------------------------------------------------------*/
#define LEDn                  			4
#define LED1_GPIO_PORT       				GPIOC
#define LED1_GPIO_CLK        				RCC_APB2Periph_GPIOC
#define LED1_GPIO_PIN        				GPIO_Pin_9
#define LED2_GPIO_PORT       				GPIOC
#define LED2_GPIO_CLK        				RCC_APB2Periph_GPIOC
#define LED2_GPIO_PIN        				GPIO_Pin_8
#define LGREEN_GPIO_PORT       			GPIOB
#define LGREEN_GPIO_CLK        			RCC_APB2Periph_GPIOB
#define LGREEN_GPIO_PIN        			GPIO_Pin_8
#define LBLUE_GPIO_PORT       			GPIOB
#define LBLUE_GPIO_CLK        			RCC_APB2Periph_GPIOB
#define LBLUE_GPIO_PIN        			GPIO_Pin_9

/* LED defines ---------------------------------------------------------------*/
/* LED defines ---------------------------------------------------------------*/
#define BEEP_PORT             			GPIOA
#define BEEP_CLK              			RCC_APB2Periph_GPIOA
#define BEEP_PIN              			GPIO_Pin_8
#define BEEP_EN()										{GPIO_SetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_DISEN()								{GPIO_ResetBits(BEEP_PORT, BEEP_PIN);}
#define BEEP_TOGGLE()								{BEEP_PORT->ODR ^= BEEP_PIN;}

/* USART defines -------------------------------------------------------------*/
/* USART defines -------------------------------------------------------------*/
#define USARTpos              			USART2
#define USARTpos_GPIO         			GPIOA
#define USARTpos_CLK          			RCC_APB1Periph_USART2
#define USARTpos_GPIO_CLK     			RCC_APB2Periph_GPIOA
#define USARTpos_RxPin        			GPIO_Pin_3
#define USARTpos_TxPin        			GPIO_Pin_2
#define USARTpos_IRQn         			USART2_IRQn
#define USARTpos_IRQHandler   			USART2_IRQHandler

/* SPI defines ---------------------------------------------------------------*/
/* SPI defines ---------------------------------------------------------------*/

//NRF1---------------------------------------------------------//
#define SPI_MISO_PORT              	GPIOA
#define SPI_MISO_CLK               	RCC_APB2Periph_GPIOA
#define SPI_MISO_PIN               	GPIO_Pin_6

#define SPI_MOSI_PORT              	GPIOA
#define SPI_MOSI_CLK               	RCC_APB2Periph_GPIOA
#define SPI_MOSI_PIN               	GPIO_Pin_7

#define SPI_SCK_PORT              	GPIOA
#define SPI_SCK_CLK               	RCC_APB2Periph_GPIOA
#define SPI_SCK_PIN               	GPIO_Pin_5

#define SPI_CSN_PORT              	GPIOA
#define SPI_CSN_CLK               	RCC_APB2Periph_GPIOA
#define SPI_CSN_PIN               	GPIO_Pin_4

#define SPI_CE_PORT              		GPIOC
#define SPI_CE_CLK               		RCC_APB2Periph_GPIOC
#define SPI_CE_PIN               		GPIO_Pin_4

#define SPI_IRQ_PORT              	GPIOC
#define SPI_IRQ_CLK               	RCC_APB2Periph_GPIOC
#define SPI_IRQ_PIN               	GPIO_Pin_5

#define EXTI_LINE_RFIRQ 						EXTI_Line5
#define RFIRQ_PinSource 						GPIO_PinSource5
#define RFIRQ_PortSource 						GPIO_PortSourceGPIOC
#define RFIRQ_EXTI_IRQn							EXTI9_5_IRQn
#define RFIRQ_EXTI_IRQHandler				EXTI9_5_IRQHandler

//NRF2---------------------------------------------------------//
#define SPI_MISO_PORT_2             GPIOB
#define SPI_MISO_CLK_2              RCC_APB2Periph_GPIOB
#define SPI_MISO_PIN_2              GPIO_Pin_14

#define SPI_MOSI_PORT_2             GPIOB
#define SPI_MOSI_CLK_2              RCC_APB2Periph_GPIOB
#define SPI_MOSI_PIN_2              GPIO_Pin_15

#define SPI_SCK_PORT_2             	GPIOB
#define SPI_SCK_CLK_2               RCC_APB2Periph_GPIOB
#define SPI_SCK_PIN_2               GPIO_Pin_13

#define SPI_CSN_PORT_2              GPIOB
#define SPI_CSN_CLK_2               RCC_APB2Periph_GPIOB
#define SPI_CSN_PIN_2               GPIO_Pin_12

#define SPI_CE_PORT_2              	GPIOC
#define SPI_CE_CLK_2               	RCC_APB2Periph_GPIOC
#define SPI_CE_PIN_2               	GPIO_Pin_6

#define SPI_IRQ_PORT_2              GPIOC
#define SPI_IRQ_CLK_2               RCC_APB2Periph_GPIOC
#define SPI_IRQ_PIN_2               GPIO_Pin_7

#define EXTI_LINE_RFIRQ_2 					EXTI_Line7
#define RFIRQ_PinSource_2 					GPIO_PinSource7
#define RFIRQ_PortSource_2 					GPIO_PortSourceGPIOC
#define RFIRQ_EXTI_IRQn_2						EXTI9_5_IRQn
#define RFIRQ_EXTI_IRQHandler_2			EXTI9_5_IRQHandler

#define HMC273MS10G_LOW_GPIO				GPIOC
#define HMC273MS10G_HIGH_GPIO				GPIOA
#define HMC273MS10G_LOW_CLK     		RCC_APB2Periph_GPIOC
#define HMC273MS10G_HIGH_CLK    		RCC_APB2Periph_GPIOA
#define HMC273_1dB									GPIO_Pin_1
#define HMC273_2dB									GPIO_Pin_2
#define HMC273_4dB									GPIO_Pin_3
#define HMC273_8dB									GPIO_Pin_0
#define HMC273_16dB									GPIO_Pin_1

#define HMC273_05db_PORT						GPIOC
#define HMC273_05db_CLK							RCC_APB2Periph_GPIOC
#define HMC273_05db_PIN							GPIO_Pin_0

#define SKY12347_LE_PORT						GPIOC
#define SKY12347_LE_CLK							RCC_APB2Periph_GPIOC
#define SKY12347_LE_PIN							GPIO_Pin_13

#define HMC273MS10G_LOW_ON(_Pin_)		{GPIO_ResetBits(HMC273MS10G_LOW_GPIO,_Pin_);}
#define HMC273MS10G_LOW_OFF(_Pin_)	{GPIO_SetBits(HMC273MS10G_LOW_GPIO,_Pin_);}
#define HMC273MS10G_HIGH_ON(_Pin_)	{GPIO_ResetBits(HMC273MS10G_HIGH_GPIO,_Pin_);}
#define HMC273MS10G_HIGH_OFF(_Pin_)	{GPIO_SetBits(HMC273MS10G_HIGH_GPIO,_Pin_);}

/* Exported functions ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void RCC_Configuration(void);
void systickInit(void);
void systDelayIntHandler(void);
void systick_delay1ms(uint32_t ulNms);

/* LED functions ------------------------------------------------------------ */
/* LED functions ------------------------------------------------------------ */
void ledInit(Led_TypeDef Led);
void ledOn(Led_TypeDef Led);
void ledOff(Led_TypeDef Led);
void ledToggle(Led_TypeDef Led);
void Delay100Us(void);
void Delay1Ms(void);
void Delay10Ms(void);
void Delay100Ms(void);
void Delay1s(void);
void USART1_Send_DMA(const uint8_t *ulBufBase, uint16_t usLen);
void USART2_Send_DMA(const uint8_t *ulBufBase, uint16_t usLen);

/* USART functions ---------------------------------------------------------- */
/* USART functions ---------------------------------------------------------- */
void uartInit(void);
void Usart1_Init(void);
void Usart2_Init(void);
int fputc(int ch, FILE *f);
int fgetc(FILE *f);
char *itoa(int value, char *string, int radix);
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...);

/* SPI functions ------------------------------------------------------------ */
/* SPI functions ------------------------------------------------------------ */
void spiInit_NRF1(void);
void spiInit_NRF2(void);
void GPIOInit_BEEP(void);
void GPIOInit_RS485(void);
void GPIOInit_Wiegand(void);
void GPIOInit_ShuaiJianQi(void);
void GPIOInit_SE2431L(void);
void SE2431L_LNA(void);
void SE2431L_Bypass(void);
void SE2431L_TX(void);

/* Other functions ---------------------------------------------------------- */
/* Other functions ---------------------------------------------------------- */
void TIM_Configuration(void);
void TIM_Configuration_TIM3(void);
void TIM_Configuration_TIM4(void);
uint8_t CalXOR(uint8_t *data, uint16_t length);
void SetExtraPWR(uint8_t uTmp);
void HMC273MS10G_PIN_Init(void);

void IWDG_Configuration(void);
void GenerateSystemReset(void);

void nrf24InitConf_NRF1(void);
void nrf24InitConf_NRF2(void);
void ConstInit(void);
void ConstInit_2(void);
void USART2_DMA_Process(void);
void USART1_DMA_Process(void);
void USART1_DMA_Process_MCUIDWrong(void);

void NVIC_Configuration(void);
void NVIC_Configuration_RFIRQ(void);
void NVIC_Configuration_TIM2(void);
void NVIC_Configuration_TIM3(void);
void NVIC_Configuration_TIM4(void);
void NVIC_Configuration_USART1(void);
void NVIC_Configuration_USART2(void);
void NVIC_Configuration_DMA_Channel4(void);
void NVIC_Configuration_DMA_Channel5(void);
void NVIC_Configuration_DMA_Channel6(void);
void NVIC_Configuration_DMA_Channel7(void);

void MY_DMA_Configuration(DMA_Channel_TypeDef *DMA_CHx, uint32_t ulPeriphBase, uint32_t ulBufBase, uint16_t usLen);
void MY_DMA_Enable(DMA_Channel_TypeDef *DMA_CHx);
void USART_TX_DMA_Configuration_Channel4(uint32_t ulBufBase, uint16_t usLen);
void USART_TX_DMA_Enable_Channel4(void);
void USART_TX_DMA_Configuration_Channel7(uint32_t ulBufBase, uint16_t usLen);
void USART_TX_DMA_Enable_Channel7(void);
void USART_RX_DMA_Configuration_Channel5(uint32_t ulBufBase, uint16_t usLen);
void USART_RX_DMA_Enable_Channel5(void);
void USART_RX_DMA_Configuration_Channel6(uint32_t ulBufBase, uint16_t usLen);
void USART_RX_DMA_Enable_Channel6(void);

/* Main functions ----------------------------------------------------------- */
/* Main functions ----------------------------------------------------------- */
void NRF_Module_Set(void);
void Read_Flash_Para(void);
void Platform_Init(void);
void NRF_Module_Init(void);
void DealRevCmdFromBoot(uint8_t *uPt);

void UID_BcdToHex(uint8_t *vpt);
void Parameter_Read_Set(void);
void Set_ShuaiJian(uint8_t *ucp_shj);
void RS485_SendData(const uint8_t *uPtData, uint16_t usDataLen);
void RS485Process_WieGand(void);
void Wiegand_Process(void);

void Adc_Init_Self(void);
uint16_t Get_Adc(uint8_t ch);
uint8_t Random_Real_Get(void);
uint16_t get_random_seed(void);
void set_random_seed( uint16_t hwSeed );
uint16_t get_random_u16( void );
uint8_t get_random_u8( void );

void Wiegand_UID_AssignClass(void);
void Wiegand_UID_TimeCountDown(void);
void BeiDong_UID_AssignClass(void);
uint8_t BeiDong_UID_AssembleSendBuf(void);
void Wiegand_DMA_Process(void);
void CMD_FromPc_ByUSART1(void);
void nrf24InitConf_NRF1_DUT(void);
void nrf24InitConf_NRF2_DUT(void);

void delay_init(u8 SYSCLK);
void delay_ms(u16 nms);
void delay_us(u32 Nus);

void nrf24InitConf_NRF1_SendMode(void);
void nrf24InitConf_NRF2_SendMode(void);
void nrf24SendPacket(const uint8_t *puBuf, uint8_t uWidth);
void nrf24SendPacket_NRF2(const uint8_t *puBuf, uint8_t uWidth);
void MCU_ID_Sum(uint32_t *ulpinout);

#endif


/*******************END OF FILE***********************************************/









