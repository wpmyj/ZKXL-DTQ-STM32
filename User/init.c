/**
  ******************************************************************************
  * @file   	init.c
  * @author  	Tian erjun
  * @version 	V1.0.0.0
  * @date   	2015.11.05
  * @brief   	platform init functions
  ******************************************************************************
  */
  
#include "main.h"
#include "mcu_config.h"

/*******************************************************************************
  * @brief  硬件平台初始化
  * @param  None
  * @retval None
  * @note 	None		  
*******************************************************************************/
void Platform_Init(void)
{
	uint8_t temp = 0;
	
	/* disable all IRQ */
	DISABLE_ALL_IRQ();
	
	/* initialize system clock */
	SysClockInit();
	
	/* initialize gpio port */
	GpioInit();
	
	Usart1_Init();
	GPIOInit_SE2431L();
	
	/* initialize the spi interface with nrf51822 */
	nrf51822_spi_init();	
	nrf51822_parameters_init();

	/* 接收器重发定时,返回ACK随机延时0~255ms，所以这个值要大于255ms	*/
	TIM3_Int_Init(NRF_RETRANSMIT_DELAY,64000);
	
	
	/* eeprom init and white_list init*/
	Fee_Init();
	
	white_len = get_len_of_white_list();
	DebugLog("\r\n[%s]:White list len = %d \r\n",__func__, white_len);
	white_on_off = get_switch_status_of_white_list();
	DebugLog("[%s]:White list switch status is %d \r\n",__func__, white_on_off);
	
	/* 复位并初始化RC500 */
	GPIOInit_MFRC500();
	temp = PcdReset();															
	
	/* enable all IRQ */
	ENABLE_ALL_IRQ();
	
	/* led 、蜂鸣器声音提示初始化完毕 */
//BEEP_EN();																	    
	ledOn(LGREEN);																 
	ledOn(LBLUE);																    
	DelayMs(200);
//BEEP_DISEN();
	ledOff(LGREEN);
	ledOff(LBLUE);
		
	/* get mcu uuid */
	get_mcu_uid();
	
	DebugLog("[%s]:System clock freq is %dMHz\r\n",__func__, SystemCoreClock / 1000000);
	DebugLog("[%s]:UID is %X%X%X%X%X%X%X%X\r\n",__func__,
	         jsq_uid[0],jsq_uid[1],jsq_uid[2],jsq_uid[3],
					 jsq_uid[4],jsq_uid[5],jsq_uid[6],jsq_uid[7]);           
					 
	if(temp)
	{
		DebugLog("[%s]:MFRC 500 reset error\r\n",__func__);
	}
	else
	{
		/* 初始化后关闭天线 */
		PcdAntennaOff();
		DebugLog("[%s]:MFRC 500 reset ok\r\n",__func__);
	}
#ifdef ENABLE_WATCHDOG
	DebugLog("[%s]:watchdog enable\r\n",__func__);
#else
	DebugLog("[%s]:watchdog disable\r\n",__func__);	
#endif //ENABLE_WATCHDOG
	DebugLog("[%s]:All peripherals init ok\r\n",__func__);
	
}

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
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART1pos_CLK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART1pos_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART1pos_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART1pos_GPIO, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART1pos, &USART_InitStructure);
	
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART1_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART1_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);
	
	/* Enable the USART1 */
	USART_Cmd(USART1pos, ENABLE);
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
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(USART2pos_CLK , ENABLE);

	GPIO_InitStructure.GPIO_Pin = USART2pos_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(USART2pos_GPIO, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = USART2pos_RxPin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(USART2pos_GPIO, &GPIO_InitStructure);

	USART_InitStructure.USART_BaudRate = UART_BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	/* Configure USART1 */
	USART_Init(USART2pos, &USART_InitStructure);
	
	NVIC_PriorityGroupConfig(SYSTEM_MVIC_GROUP_SET);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = UART2_PREEMPTION_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = UART2_SUB_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//中断配置..Only IDLE Interrupt..
	USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);
	
	/* Enable the USART1 */
	USART_Cmd(USART2pos, ENABLE);
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
	USART_SendData(USART1pos, (u8) ch);

	/* Loop until the end of transmission */
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_TXE) == SET))
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
	while(!(USART_GetFlagStatus(USART1pos, USART_FLAG_RXNE) == SET))
	{
	}

	/* Read a character from the USART and RETURN */
	return (USART_ReceiveData(USART1pos));
}

/* SPI Functions ------------------------------------------------------------ */

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

//#ifndef TX_MODE		
	SE2431L_Bypass();
////	SE2431L_LNA();
//#else	
//	SE2431L_TX();
//#endif //TX_MODE	
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



void GPIOInit_MFRC500(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
	 /* Configure MFRC500 PIN */
    GPIO_InitStructure.GPIO_Pin = MFRC500_PD_Pin|MFRC500_ALE_Pin|MFRC500_CS_Pin|MFRC500_IRQ_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC500_Control_Port, &GPIO_InitStructure);
    GPIO_WriteBit(MFRC500_Control_Port, MFRC500_PD_Pin, Bit_RESET);
    GPIO_WriteBit(MFRC500_Control_Port, MFRC500_ALE_Pin, Bit_RESET);
    GPIO_WriteBit(MFRC500_Control_Port, MFRC500_CS_Pin, Bit_SET);
    GPIO_WriteBit(MFRC500_Control_Port, MFRC500_IRQ_Pin, Bit_RESET);

    GPIO_InitStructure.GPIO_Pin = MFRC500_WR_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC500_RDWR_Port, &GPIO_InitStructure);
    GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_WR_Pin, Bit_SET);
   // GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_RD_Pin, Bit_SET);

    GPIO_InitStructure.GPIO_Pin = MFRC500_RD_Pin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC500_RDWR_Port, &GPIO_InitStructure);
  //  GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_WR_Pin, Bit_SET);
    GPIO_WriteBit(MFRC500_RDWR_Port, MFRC500_RD_Pin, Bit_SET);


    GPIO_InitStructure.GPIO_Pin = MFRC500_DATA_Pin;	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(MFRC500_DATA_Port, &GPIO_InitStructure);
    GPIO_WriteBit(MFRC500_DATA_Port, MFRC500_DATA_Pin, Bit_RESET);
}

/**************************************END OF FILE****************************/

