/**
  ******************************************************************************
  * @file   	rfsim_MCUConf.c
  * @author  	Tim.Xia
  * @version 	V2.0.0
  * @date   	31-5-2013
  * @brief
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "rfsim_MCUConf.h"
#include "string.h"
#include "FlashRW.h"

/* Exported Variables --------------------------------------------------------*/
/* Exported Variables --------------------------------------------------------*/
/* Exported Variables --------------------------------------------------------*/

/* LED Variables -------------------------------------------------------------*/
/* LED Variables -------------------------------------------------------------*/
GPIO_TypeDef *GPIO_PORT[LEDn] = {	LED1_GPIO_PORT,  LED2_GPIO_PORT, LGREEN_GPIO_PORT, LBLUE_GPIO_PORT};
const uint16_t GPIO_PIN[LEDn] = {	LED1_GPIO_PIN, LED2_GPIO_PIN, LGREEN_GPIO_PIN, LBLUE_GPIO_PIN};
const uint32_t GPIO_CLK[LEDn] = {	LED1_GPIO_CLK, LED2_GPIO_CLK, LGREEN_GPIO_CLK, LBLUE_GPIO_CLK};

/* Exported Functions ------------------------------------------------------- */
/* Exported Functions ------------------------------------------------------- */
/* Exported Functions ------------------------------------------------------- */

/* LED Functions ------------------------------------------------------------ */
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
void ledOff(Led_TypeDef Led)
{
	if((Led == LED1) || (Led == LED2))
		GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
	else if((Led == LGREEN) || (Led == LBLUE))
		GPIO_PORT[Led]->BRR = GPIO_PIN[Led];
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
	else if((Led == LGREEN) || (Led == LBLUE))
		GPIO_PORT[Led]->BSRR = GPIO_PIN[Led];
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

/*
 *@ 使用SysTick的普通计数模式对延迟进行管理
 *@ 包括delay_us,delay_ms  
 *@ 修正了中断中调用出现死循环的错误
 *@ 防止延时不准确,采用do while结构! 
 */
static uint8_t  fac_us = 0x00;        //us延时倍乘数
static uint16_t fac_ms = 0x00;        //ms延时倍乘数
/*初始化延迟函数*/
void delay_init(uint8_t SYSCLK)
{
	SysTick->CTRL&=0xfffffffb;          //选择内部时钟 HCLK/8
	fac_us=SYSCLK/8;		    
	fac_ms=(uint16_t)fac_us*1000;
}								    
/*
 *@ 延时Nms
 *@ 注意Nms的范围
 *@ Nms<=0xffffff*8/SYSCLK
 *@ 对72M条件下,Nms<=1864 
 */
void delay_ms(uint16_t nms)
{	 		  	  
	uint32_t temp;		   
	SysTick->LOAD=(uint32_t)nms*fac_ms;       //时间加载
	SysTick->VAL =0x00;                  //清空计数器
	SysTick->CTRL=0x01 ;                 //开始倒数  
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));    //等待时间到达
	SysTick->CTRL=0x00;                   //关闭计数器
	SysTick->VAL =0X00;                   //清空计数器	  	    
}   
/*延时Nus*/								   
void delay_us(uint32_t Nus)
{		
	uint32_t temp;	    	 
	SysTick->LOAD=Nus*fac_us;             //时间加载	  		 
	SysTick->VAL=0x00;                    //清空计数器
	SysTick->CTRL=0x01 ;                  //开始倒数 	 
	do
	{
		temp=SysTick->CTRL;
	}
	while(temp&0x01&&!(temp&(1<<16)));    //等待时间到达   
	SysTick->CTRL=0x00;                   //关闭计数器
	SysTick->VAL =0X00;                   //清空计数器	    
}  

extern bool gbf_hse_setup_fail;
/*******************************************************************************
  * @brief  The Whole Board Init.
  * @param  None
  * @retval None
*******************************************************************************/
void RCC_Configuration(void)
{
	SystemInit();
	systickInit();

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

/* Delay Exported Function ---------------------------------------------------*/
/* Delay Exported Function ---------------------------------------------------*/

/*******************************************************************************
  * @brief  Initialize SysTick
  * @param  None
  * @retval None
*******************************************************************************/
extern bool gbf_hse_setup_fail;
void systickInit(void)
{
	if (gbf_hse_setup_fail == FALSE)
	{
		if (SysTick_Config(72000000 / 1000))
			while (1);
		// delay_init(72);
	}
	else
	{
		if (SysTick_Config(64000000 / 1000))
			while (1);
		// delay_init(64);
	}
}

volatile uint32_t GulDelayCount = 0;				/* SysTick Counter.Global VARIABLE		*/
/*******************************************************************************
  * @brief  Delay few miliseconds.
  * @param  NMS: numbers of your want delay ms.
  * @retval None
*******************************************************************************/
void systick_delay1ms(uint32_t ulNms)
{
	GulDelayCount = ulNms;
	while (GulDelayCount);
}

/*******************************************************************************
  * @brief  ISR Handler of SysTick
  * @param  None
  * @retval None
*******************************************************************************/
void systDelayIntHandler(void)
{
	if (GulDelayCount-- != 0x00)
	{
	}
}

void Delay100Us(void)
{
	volatile uint16_t i = 2000;
	while (i--);
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
extern volatile bool vgbf_usart1_dma_can_send;
extern volatile uint8_t vguct_b1s_u1dma_canbeused_cnt;
void USART1_Send_DMA(const uint8_t *ulBufBase, uint16_t usLen)
{
	RESET_COUNTER(vguct_b1s_u1dma_canbeused_cnt);
	RESET_FLAGS(vgbf_usart1_dma_can_send);
	DMA_Cmd(DMA1_Channel4, DISABLE);
	Delay100Us();
	USART_TX_DMA_Configuration_Channel4((uint32_t)ulBufBase, usLen);
	USART_TX_DMA_Enable_Channel4();
}

extern volatile bool vgbf_usart2_dma_can_send;
extern volatile uint8_t vguct_b1s_u2dma_canbeused_cnt;
void USART2_Send_DMA(const uint8_t *ulBufBase, uint16_t usLen)
{
	RESET_COUNTER(vguct_b1s_u2dma_canbeused_cnt);
	RESET_FLAGS(vgbf_usart2_dma_can_send);
	DMA_Cmd(DMA1_Channel7, DISABLE);
	Delay100Us();
	USART_TX_DMA_Configuration_Channel7((uint32_t)ulBufBase, usLen);
	USART_TX_DMA_Enable_Channel7();
}

/* USART Functions ---------------------------------------------------------- */
/* USART Functions ---------------------------------------------------------- */

/****************************************************************************
* 名    称：void Usart1_Init(void)
* 功    能：串口1初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void Usart1_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART1, &USART_InitStructure);
	/* Enable the USART1 */
	USART_Cmd(USART1, ENABLE);
}

/****************************************************************************
* 名    称：void Usart2_Init(void)
* 功    能：串口2初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无
****************************************************************************/
void Usart2_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 , ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART2 */
	USART_Init(USART2, &USART_InitStructure);
	/* Enable the USART2 */
	USART_Cmd(USART2, ENABLE);
}

/* TIM2 Functions ----------------------------------------------------------- */
/* TIM2 Functions ----------------------------------------------------------- */
void IWDG_Configuration(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);		/* 写入0x5555,用于允许狗狗寄存器写入功能 */
	IWDG_SetPrescaler(IWDG_Prescaler_256);					/* 狗狗时钟分频,40K/256=156HZ(6.4ms)*/
	IWDG_SetReload(WDT_2S_256PRE);									/* 喂狗时间 1s/6.4MS=156 .注意不能大于0xfff*/
	IWDG_ReloadCounter();														/* 喂狗*/
	IWDG_Enable();																	/* 使能*/
}

/* SPI Functions ------------------------------------------------------------ */
/* SPI Functions ------------------------------------------------------------ */

void spiInit_NRF1(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	GPIO_InitStructure.GPIO_Pin   = SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MISO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_MOSI_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MOSI_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_SCK_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_CE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CE_PORT, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_IRQ_PIN;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_IRQ_PORT, &GPIO_InitStructure);

	GPIO_EXTILineConfig(RFIRQ_PortSource, RFIRQ_PinSource);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Line = EXTI_LINE_RFIRQ;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_4;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_Cmd(SPI1, ENABLE);
	GPIO_SetBits(SPI_CSN_PORT, SPI_CSN_PIN);
}
/*******************************************************************************
  * @brief  Configures STM32F103RBT SPI2 port.
  * @param  None
  * @retval None
*******************************************************************************/
void spiInit_NRF2(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	SPI_InitTypeDef  SPI_InitStructure;

	GPIO_InitStructure.GPIO_Pin   = SPI_MISO_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MISO_PORT_2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_MOSI_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_MOSI_PORT_2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_SCK_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(SPI_SCK_PORT_2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_CSN_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CSN_PORT_2, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = SPI_CE_PIN_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
	GPIO_Init(SPI_CE_PORT_2, &GPIO_InitStructure);

	/* Configure SPI_IRQ Pin */
	GPIO_InitStructure.GPIO_Pin   = SPI_IRQ_PIN_2;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(SPI_IRQ_PORT_2, &GPIO_InitStructure);

	GPIO_EXTILineConfig(RFIRQ_PortSource_2, RFIRQ_PinSource_2);
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_Line = EXTI_LINE_RFIRQ_2;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;

	SPI_Init(SPI2, &SPI_InitStructure);
	SPI_Cmd(SPI2, ENABLE);
	GPIO_SetBits(SPI_CSN_PORT_2, SPI_CSN_PIN_2);
}

void GPIOInit_RS485(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* Configure RS485 EN Port */
	GPIO_InitStructure.GPIO_Pin = RS485_EN_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(RS485_EN_PORT, &GPIO_InitStructure);
	
	RS485_REV_MODE();
}

void GPIOInit_Wiegand(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Configure WIEGAND DATA0 output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure); 
	
	/* Configure WIEGAND DATA1 output push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}

extern volatile bool gbf_rs485_can_send;
void RS485_SendData(const uint8_t *uPtData, uint16_t usDataLen)
{
	RS485_SEND_MODE();
	DMA_ClearFlag(DMA1_FLAG_TC4);
	USART1_Send_DMA(uPtData, usDataLen);
	gbf_rs485_can_send = FALSE;
}

void GPIOInit_BEEP(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(BEEP_PORT, &GPIO_InitStructure);	
}

void GPIOInit_ShuaiJianQi(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = SKY12347_LE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SKY12347_LE_PORT, &GPIO_InitStructure);	
	
	/* Configure HMC273_0_5dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_05db_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_LOW_GPIO, &GPIO_InitStructure);
	/* Configure HMC273_1dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_1dB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_LOW_GPIO, &GPIO_InitStructure);
	/* Configure HMC273_2dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_2dB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_LOW_GPIO, &GPIO_InitStructure);
	/* Configure HMC273_4dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_4dB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_LOW_GPIO, &GPIO_InitStructure);
	/* Configure HMC273_8dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_8dB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_HIGH_GPIO, &GPIO_InitStructure);
	/* Configure HMC273_16dB */
	GPIO_InitStructure.GPIO_Pin = HMC273_16dB;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(HMC273MS10G_HIGH_GPIO, &GPIO_InitStructure);
	
	GPIO_SetBits(SKY12347_LE_PORT, SKY12347_LE_PIN);
	GPIO_SetBits(HMC273_05db_PORT, HMC273_05db_PIN);
}

/* NVIC Functions ----------------------------------------------------------- */
/* NVIC Functions ----------------------------------------------------------- */
/*******************************************************************************
  * @brief  Configures NVIC.
  * @param  None
  * @retval None
*******************************************************************************/
void NVIC_Configuration_RFIRQ(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);

	NVIC_InitStructure.NVIC_IRQChannel = RFIRQ_EXTI_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_TIM2(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);

	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	NVIC_ClearPendingIRQ(TIM2_IRQn);
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_TIM3(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	NVIC_ClearPendingIRQ(TIM3_IRQn);
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_TIM4(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	NVIC_ClearPendingIRQ(TIM4_IRQn);
	/* Enable the TIM2 gloabal Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_USART1(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART1,USART_IT_TC,DISABLE);
	USART_ITConfig(USART1,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
}

void NVIC_Configuration_USART2(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART2,USART_IT_TC,DISABLE);
	USART_ITConfig(USART2,USART_IT_RXNE,DISABLE);
	USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
}

void NVIC_Configuration_DMA_Channel4(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_DMA_Channel5(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_DMA_Channel6(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void NVIC_Configuration_DMA_Channel7(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_Self);
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/* TIM2 Functions ----------------------------------------------------------- */
/* TIM2 Functions ----------------------------------------------------------- */
void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_DeInit(TIM2);
	TIM_ClearITPendingBit(TIM2, TIM_IT_CC1);
	// Time base configuration
	if (gbf_hse_setup_fail == FALSE)
		TIM_TimeBaseStructure.TIM_Prescaler = (72000000 / 2000 - 1);
	else
		TIM_TimeBaseStructure.TIM_Prescaler = (64000000 / 2000 - 1);
	TIM_TimeBaseStructure.TIM_Period = 20 - 1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

/* TIM3 Functions ----------------------------------------------------------- */
/* TIM3 Functions ----------------------------------------------------------- */
void TIM_Configuration_TIM3(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	TIM_DeInit(TIM3);
	TIM_ClearITPendingBit(TIM3, TIM_IT_CC1);
	TIM_TimeBaseStructure.TIM_Period = Tim3TimePiece;
	TIM_TimeBaseStructure.TIM_Prescaler = TIM3_timPrescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;

	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	// TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	// TIM_Cmd(TIM3, ENABLE);
}

/* TIM4 Functions ----------------------------------------------------------- */
/* TIM4 Functions ----------------------------------------------------------- */
void TIM_Configuration_TIM4(void)
{
	TIM_TimeBaseInitTypeDef	TIM_TimeBaseStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_DeInit(TIM4);
	TIM_ClearITPendingBit(TIM4, TIM_IT_CC1);
	// Time base configuration--延时1ms--以500us为基准
	TIM_TimeBaseStructure.TIM_Period = 2 - 1;
	// TIM_TimeBaseStructure.TIM_Prescaler = timPrescaler;
	if(gbf_hse_setup_fail == FALSE)
		TIM_TimeBaseStructure.TIM_Prescaler = (72000000 / 2000 - 1);
	else
		TIM_TimeBaseStructure.TIM_Prescaler = (64000000 / 2000 - 1);
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	// TIM IT enable 
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
  // TIM4 enable counter
	TIM_Cmd(TIM4, ENABLE);
}

/* DMA Functions ------------------------------------------------------------ */
/* DMA Functions ------------------------------------------------------------ */
/*******************************************************************************
  * @brief  Configures NVIC.
  * @param  None
  * @retval None
*******************************************************************************/
volatile uint16_t USART_TX_DMA_PAYLOAD_WIDTH_Channel4 = 0;
void USART_TX_DMA_Configuration_Channel4(uint32_t ulBufBase, uint16_t usLen)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel4);
	USART_TX_DMA_PAYLOAD_WIDTH_Channel4 = usLen;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ulBufBase;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = usLen;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	USART_DMACmd(USART1,USART_DMAReq_Tx,ENABLE);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel4, DMA_IT_TE, ENABLE);
}
void USART_TX_DMA_Enable_Channel4(void)
{
	DMA_Cmd(DMA1_Channel4, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel4, USART_TX_DMA_PAYLOAD_WIDTH_Channel4);
	DMA_Cmd(DMA1_Channel4, ENABLE);
}

volatile uint16_t USART_TX_DMA_PAYLOAD_WIDTH_Channel7 = 0;
void USART_TX_DMA_Configuration_Channel7(uint32_t ulBufBase, uint16_t usLen)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel7);
	USART_TX_DMA_PAYLOAD_WIDTH_Channel7 = usLen;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ulBufBase;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = usLen;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel7, &DMA_InitStructure);
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel7, DMA_IT_TE, ENABLE);
	DMA_Cmd(DMA1_Channel7, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7, usLen);
	DMA_Cmd(DMA1_Channel7, ENABLE);
}
void USART_TX_DMA_Enable_Channel7(void)
{
	DMA_Cmd(DMA1_Channel7, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel7, USART_TX_DMA_PAYLOAD_WIDTH_Channel7);
	DMA_Cmd(DMA1_Channel7, ENABLE);
}

volatile uint16_t USART_RX_DMA_PAYLOAD_WIDTH_Channel5 = 0;
void USART_RX_DMA_Configuration_Channel5(uint32_t ulBufBase, uint16_t usLen)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel5);
	USART_RX_DMA_PAYLOAD_WIDTH_Channel5 = usLen;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ulBufBase;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = usLen;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TE, ENABLE);
}
void USART_RX_DMA_Enable_Channel5(void)
{
	DMA_Cmd(DMA1_Channel5, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel5, USART_RX_DMA_PAYLOAD_WIDTH_Channel5);
	DMA_Cmd(DMA1_Channel5, ENABLE);
}

volatile uint16_t USART_RX_DMA_PAYLOAD_WIDTH_Channel6 = 0;
void USART_RX_DMA_Configuration_Channel6(uint32_t ulBufBase, uint16_t usLen)
{
	DMA_InitTypeDef DMA_InitStructure;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	DMA_DeInit(DMA1_Channel6);
	USART_RX_DMA_PAYLOAD_WIDTH_Channel6 = usLen;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&USART2->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ulBufBase;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = usLen;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel6, &DMA_InitStructure);
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TC, ENABLE);
	DMA_ITConfig(DMA1_Channel6, DMA_IT_TE, ENABLE);
}
void USART_RX_DMA_Enable_Channel6(void)
{
	DMA_Cmd(DMA1_Channel6, DISABLE);
	DMA_SetCurrDataCounter(DMA1_Channel6, USART_RX_DMA_PAYLOAD_WIDTH_Channel6);
	DMA_Cmd(DMA1_Channel6, ENABLE);
}

/*********************************************************************************
**功	能：异或计算函数
**输    入: data	待计算的数据块
**		  : length	数据块长度
**返	回：异或计算结果
**备	注：NULL
*********************************************************************************/
uint8_t CalXOR(uint8_t *data, uint16_t length)
{
	uint8_t temp_xor;
	uint16_t i;

	temp_xor = *data;
	for (i = 1;i < length; i++)
	{
		temp_xor = temp_xor ^ *(data + i);
	}
	return temp_xor;
}

void UID_BcdToHex(uint8_t *vpt)
{
	uint8_t ucTmp = 0x00;
	uint32_t ulTmp = 0x00000000;
	{
		ucTmp = *(vpt + 0);
		ucTmp = (ucTmp >> 4) * 10 + (ucTmp & 0x0F);
		ulTmp += ucTmp * 100000000;
	}
	{
		ucTmp = *(vpt + 1);
		ucTmp = (ucTmp >> 4) * 10 + (ucTmp & 0x0F);
		ulTmp += ucTmp * 1000000;
	}
	{
		ucTmp = *(vpt + 2);
		ucTmp = (ucTmp >> 4) * 10 + (ucTmp & 0x0F);
		ulTmp += ucTmp * 10000;
	}
	{
		ucTmp = *(vpt + 3);
		ucTmp = (ucTmp >> 4) * 10 + (ucTmp & 0x0F);
		ulTmp += ucTmp * 100;
	}
	{
		ucTmp = *(vpt + 4);
		ucTmp = (ucTmp >> 4) * 10 + (ucTmp & 0x0F);
		ulTmp += ucTmp;
	}
	*(vpt + 0) = (uint8_t)(ulTmp >> 24);
	*(vpt + 1) = (uint8_t)(ulTmp >> 16);
	*(vpt + 2) = (uint8_t)(ulTmp >> 8);
	*(vpt + 3) = (uint8_t)(ulTmp);
}

void GPIOInit_SE2431L(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = SE2431L_CTX_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CTX_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CPS_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CPS_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = SE2431L_CSD_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SE2431L_CSD_PORT, &GPIO_InitStructure);
	
	SE2431L_Bypass();
	// SE2431L_LNA();
}

void SE2431L_LNA(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_Bypass(void)
{
	GPIO_ResetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_ResetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SE2431L_TX(void)
{
	GPIO_SetBits(SE2431L_CPS_PORT, SE2431L_CPS_PIN);	//110 LNA. 111 TX. 010 bypass.
	GPIO_SetBits(SE2431L_CSD_PORT, SE2431L_CSD_PIN);
	GPIO_SetBits(SE2431L_CTX_PORT, SE2431L_CTX_PIN);
}

void SetExtraPWR(uint8_t uTmp)
{
	if (uTmp > MAX_SHUAIJIAN)
		uTmp = MAX_SHUAIJIAN;
	if (uTmp & 0x01){
		HMC273MS10G_LOW_ON(HMC273_1dB);
	}else{
		HMC273MS10G_LOW_OFF(HMC273_1dB);
	}
	if (uTmp & 0x02){
		HMC273MS10G_LOW_ON(HMC273_2dB);
	}else{
		HMC273MS10G_LOW_OFF(HMC273_2dB);
	}
	if (uTmp & 0x04){
		HMC273MS10G_LOW_ON(HMC273_4dB);
	}else{
		HMC273MS10G_LOW_OFF(HMC273_4dB);
	}
	if (uTmp & 0x08){
		HMC273MS10G_HIGH_ON(HMC273_8dB);
	}else{
		HMC273MS10G_HIGH_OFF(HMC273_8dB);
	}
	if (uTmp & 0x10){
		HMC273MS10G_HIGH_ON(HMC273_16dB);
	}else{
		HMC273MS10G_HIGH_OFF(HMC273_16dB);
	}
}

void HMC273MS10G_PIN_Init(void)
{
	HMC273MS10G_LOW_OFF(HMC273_1dB);
	HMC273MS10G_LOW_OFF(HMC273_2dB);
	HMC273MS10G_LOW_OFF(HMC273_4dB);
	HMC273MS10G_HIGH_OFF(HMC273_8dB);
	HMC273MS10G_HIGH_OFF(HMC273_16dB);
}

void Adc_Init_Self(void)
{   
	ADC_InitTypeDef ADC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |
	                       RCC_APB2Periph_ADC1  , ENABLE);      //使能ADC1通道时钟
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);   //设置ADC分频因子6
	//72M/6=12,ADC最大时间不能超过14M
	//PA1 作为模拟通道输入引脚
	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;					//模拟输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);           			//初始化GPIOA.1

	ADC_DeInit(ADC1);    //复位ADC1,将外设  ADC1 的全部寄存器重设为缺省值
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;  	//ADC独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;      		//单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;  	//单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;  			//顺序进行规则转换的ADC通道的数目
	ADC_Init(ADC1, &ADC_InitStructure);      			//根据指定的参数初始化外设ADCx
	ADC_Cmd(ADC1, ENABLE);                        //使能指定的ADC1
	ADC_ResetCalibration(ADC1);                   //开启复位校准
	while (ADC_GetResetCalibrationStatus(ADC1)); 	//等待复位校准结束
	ADC_StartCalibration(ADC1);                   //开启AD校准
	while (ADC_GetCalibrationStatus(ADC1));      	//等待校准结束
}

//获得ADC值
//ch:通道值  0~3
uint16_t Get_Adc(uint8_t ch)
{
	//设置指定ADC的规则组通道，设置它们的转化顺序和采样时间
	ADC_RegularChannelConfig(ADC1, ch, 1, ADC_SampleTime_239Cycles5 );  //通道1规则采样顺序值为1,采样时间为239.5周期
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);  					//使能指定的ADC1的软件转换功能
	while (!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC ));	//等待转换结束
	return ADC_GetConversionValue(ADC1);  						//返回最近一次ADC1规则组的转换结果
}

extern FLASH_PARA_T tFlashPara;
uint8_t Random_Real_Get(void)
{
	// set_random_seed(get_random_seed());
	set_random_seed(tFlashPara.ucSelfSubID);
	return get_random_u8();
}

uint16_t get_random_seed(void)
{
	uint16_t usTmpAdcVal = 0x0000;
	uint16_t usMask = 0x8000;
	do
	{
		usTmpAdcVal |= (Get_Adc(1) & 0x0001)? usMask : 0x0000;
		usMask >>= 1;
	}while(usMask);
	return usTmpAdcVal;
}

static uint16_t s_hwRandomSeed = 0xAA55;
static uint8_t s_chRandomTable[] = {
                0x12,0x23,0x45,0x67,0x89,0xAB,0xCD,0xEF,
                0xF1,0xE2,0xD3,0xC4,0xB5,0xA6,0x97,0x88};

void set_random_seed( uint16_t hwSeed )
{
    s_hwRandomSeed ^= hwSeed;
}

uint16_t get_random_u16( void )
{
    uint16_t *phwResult = (uint16_t *)&s_chRandomTable[(s_hwRandomSeed & 0x0E)];
    
    *phwResult += s_hwRandomSeed;
    s_hwRandomSeed ^= *phwResult;   
    return *phwResult;
}

uint8_t get_random_u8( void )
{
    return get_random_u16();
}

/*******************************************************************************
* Function Name   : int fputc(int ch, FILE *f)
* Description     : Retargets the C library printf function to the USART.printf重定向
* Input           : None
* Output          : None
* Return          : None
*******************************************************************************/
int fputc(int ch, FILE *f)
{
	/* Write a character to the USART */
	USART_SendData(USART1, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == SET))
	{
	}

	return ch;
}

/*******************************************************************************
* Function Name   : int fgetc(FILE *f)
* Description     : Retargets the C library printf function to the USART.fgetc重定向
* Input           : None
* Output          : None
* Return          : 读取到的字符
*******************************************************************************/
int fgetc(FILE *f)
{
	/* Loop until received a char */
	while(!(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET))
	{
	}

	/* Read a character from the USART and RETURN */
	return (USART_ReceiveData(USART1));
}
/******************************************************
		整形数据转字符串函数
        char *itoa(int value, char *string, int radix)
		radix=10 标示是10进制	非十进制，转换结果为0;

	    例：d=-379;
		执行	itoa(d, buf, 10); 后

		buf="-379"
**********************************************************/
char *itoa(int value, char *string, int radix)
{
	int     i, d;
	int     flag = 0;
	char    *ptr = string;

	/* This implementation only works for decimal numbers. */
	if (radix != 10)
	{
		*ptr = 0;
		return string;
	}

	if (!value)
	{
		*ptr++ = 0x30;
		*ptr = 0;
		return string;
	}

	/* if this is a negative value insert the minus sign. */
	if (value < 0)
	{
		*ptr++ = '-';

		/* Make the value positive. */
		value *= -1;
	}

	for (i = 10000; i > 0; i /= 10)
	{
		d = value / i;

		if (d || flag)
		{
			*ptr++ = (char)(d + 0x30);
			value -= (d * i);
			flag = 1;
		}
	}

	/* Null terminate the string. */
	*ptr = 0;

	return string;

} /* NCL_Itoa */

/****************************************************************************
* 名    称：void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...)
* 功    能：格式化串口输出函数
* 入口参数：USARTx:  指定串口
			Data：   发送数组
			...:     不定参数
* 出口参数：无
* 说    明：格式化串口输出函数
        	"\r"	回车符	   USART_OUT(USART1, "abcdefg\r")
			"\n"	换行符	   USART_OUT(USART1, "abcdefg\r\n")
			"%s"	字符串	   USART_OUT(USART1, "字符串是：%s","abcdefg")
			"%d"	十进制	   USART_OUT(USART1, "a=%d",10)
* 调用方法：无
****************************************************************************/
void USART_OUT(USART_TypeDef* USARTx, uint8_t *Data,...){
	const char *s;
	int d;
	char buf[16];
	va_list ap;
	va_start(ap, Data);

	while (*Data!=0){				                         //判断是否到达字符串结束符
		if (*Data==0x5c){									 //'\'
			switch (*++Data){
			case 'r':							          //回车符
				USART_SendData(USARTx, 0x0d);

				Data++;
				break;
			case 'n':							          //换行符
				USART_SendData(USARTx, 0x0a);
				Data++;
				break;

			default:
				Data++;
				break;
			}


		}
		else if (*Data=='%'){
			switch (*++Data){
			case 's':										  //字符串
				s = va_arg(ap, const char *);
				for ( ; *s; s++) {
					USART_SendData(USARTx,*s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
				}
				Data++;
				break;
			case 'd':										  //十进制
				d = va_arg(ap, int);
				itoa(d, buf, 10);
				for (s = buf; *s; s++) {
					USART_SendData(USARTx,*s);
					while (USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
				}
				Data++;
				break;
			default:
				Data++;
				break;
			}
		}
		else USART_SendData(USARTx, *Data++);
		while (USART_GetFlagStatus(USARTx, USART_FLAG_TC)==RESET);
	}
}

#if defined(__CC_ARM) /*------------------RealView Compiler -----------------*/
__asm void GenerateSystemReset(void)
{
	MOV R0, #1         	//; 
	MSR FAULTMASK, R0  	//; 清除FAULTMASK 禁止一切中断产生
	LDR R0, =0xE000ED0C //;
	LDR R1, =0x05Fa0004 //; 
	STR R1, [R0]       	//; 系统软件复位   
deadloop
    B deadloop        //; 死循环使程序运行不到下面的代码
}
#elif (defined(__ICCARM__)) /*------------------ ICC Compiler -------------------*/
//#pragma diag_suppress=Pe940
void GenerateSystemReset(void)
{
	__ASM("MOV R0, #1");
	__ASM("MSR FAULTMASK, R0");
	SCB->AIRCR = 0x05FA0004;
	for (;;);
}
#endif


/* NVIC Functions ----------------------------------------------------------- */
/* NVIC Functions ----------------------------------------------------------- */



/*******************END OF FILE************************************************/









