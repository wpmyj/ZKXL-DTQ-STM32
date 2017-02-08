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
#include "nrf.h"
#include "app_timer.h"
#include "app_send_data_process.h"
#include "app_systick_package_process.h"
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

/* Private variables ---------------------------------------------------------*/
spi_cmd_type_t 			       spi_cmd_type;
nrf_communication_t	       nrf_data;
extern WhiteList_Typedef   wl;

void systick_timer_init( void );
/*******************************************************************************
  * @brief  硬件平台初始化
  * @param  None
  * @retval None
  * @note 	None
*******************************************************************************/
void Platform_Init(void)
{
	/* disable all IRQ */
	DISABLE_ALL_IRQ();

	/* initialize system clock */
	SysClockInit();

	/* initialize gpio port */
	GpioInit();

	Usart1_Init();

	/* get mcu uuid */
	get_mcu_uid();

	/* initialize the spi interface with nrf51822 */
	nrf51822_spi_init();

	/* eeprom init and white_list init*/
	Fee_Init(FEE_INIT_POWERUP);
	get_white_list_from_flash();

	/* init software timer */
	sw_timer_init();
	system_timer_init();
	send_data_process_timer_init();
	card_timer_init();

	/* 复位并初始化RC500 */
	mfrc500_init();

	/* enable all IRQ */
	ENABLE_ALL_IRQ();

	/* led 、蜂鸣器声音提示初始化完毕 */
#ifndef OPEN_SILENT_MODE
	BEEP_EN();
#endif
	ledOn(LGREEN);
	ledOn(LBLUE);
	DelayMs(200);
	BEEP_DISEN();
	ledOff(LGREEN);
	ledOff(LBLUE);
	IWDG_Configuration();
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

#ifdef ZL_RP551_MAIN_E
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
#endif

/* Private functions ---------------------------------------------------------*/
static uint8_t hal_nrf_rw(SPI_TypeDef* SPIx, uint8_t value)
{
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPIx, value);
	while(SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);
	return(SPI_I2S_ReceiveData(SPIx));
}

uint8_t uesb_nrf_get_irq_flags(SPI_TypeDef* SPIx, uint8_t *flags, uint8_t *rx_data_len, uint8_t *rx_data)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;

	*rx_data_len = 0;
	memset(spi_cmd_type.data, 0xFF, BUFFER_SIZE_MAX);
	spi_cmd_type.spi_cmd = UESB_READ_RF_INT_STATUS;
	spi_cmd_type.data_len = 0x02;
	spi_cmd_type.data[0] = 0xFF;
	spi_cmd_type.data[1] = 0xFF;
	temp_data = (uint8_t *)&spi_cmd_type;

	NRF1_CSN_LOW();	//开始SPI传输
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
		retval[i] = hal_nrf_rw(SPIx, *(temp_data+i));

		//printf("%2x ",retval[i]);
		if(i ==  2 && retval[2] != 0x00 && retval[2] != 0xFF)
		{
			*flags = retval[2];
		}

		if( i == 3 && (retval[2] & (1<<RX_DR)) && retval[3] < BUFFER_SIZE_MAX )				// If "received data ready" interrupt from hrf
	  {
			*rx_data_len = retval[3];
			spi_cmd_type.data_len += *rx_data_len;
			spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&spi_cmd_type.data[3], spi_cmd_type.data_len - 3);
		}
	}
	//printf("\r\n");
	NRF1_CSN_HIGH();	//关闭SPI传输

	memcpy(rx_data, &retval[4],*rx_data_len);
	//DELAY_FUNC(DELAY_TIME);

	if(retval[0] != 0 && retval[0] != 0xFF) 			//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

uint8_t uesb_nrf_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint16_t i = 0;
	uint8_t *temp_data = NULL;

	spi_cmd_type.spi_cmd = UESB_WRITE_TX_PAYLOAD;
	spi_cmd_type.data_len = length+2;
	spi_cmd_type.count = count;
	spi_cmd_type.delay100us = delay100us;

	memcpy(spi_cmd_type.data, tx_pload, length);
	spi_cmd_type.data[spi_cmd_type.data_len-2] = XOR_Cal((uint8_t *)&spi_cmd_type, spi_cmd_type.data_len+2);
	temp_data = (uint8_t *)&spi_cmd_type;

	NRF2_CSN_LOW();	//开始SPI传输
	memset(retval, 0, BUFFER_SIZE_MAX);
	for(i=0; i<spi_cmd_type.data_len+3; i++)
	{
#ifdef ZL_RP551_MAIN_E
	retval[i] = hal_nrf_rw(SPI1, *(temp_data+i));
#endif

#ifdef ZL_RP551_MAIN_F
		retval[i] = hal_nrf_rw(SPI2, *(temp_data+i));
#endif
	}
	NRF2_CSN_HIGH();	//关闭SPI传输

	if(retval[0] != 0) 									//若接收到数据校验正确
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/**************************************END OF FILE****************************/

