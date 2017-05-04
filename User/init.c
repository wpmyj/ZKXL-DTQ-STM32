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
#include "app_spi_send_data_process.h"
#include "app_card_process.h"

typedef struct
{
	uint8_t header;
	uint8_t cmd;
	uint8_t channel;
	uint8_t data_xor;
	uint8_t end;
}cpu_spi_cmd_typedef;
	
/* Private variables ---------------------------------------------------------*/
spi_cmd_type_t 			    spi_cmd_type;
nrf_communication_t	    nrf_data;
bool 						        gbf_hse_setup_fail = FALSE;		// 外部晶振起振标志
RF_TypeDef 				      rf_var;
extern wl_typedef       wl;
extern uint8_t dtq_self_inspection_flg;

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
	ledOn(LRED);
	ledOn(LBLUE);
	DelayMs(200);
	BEEP_DISEN();
	ledOff(LBLUE);
	clicker_config_default_set();
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

uint8_t spi_read_tx_payload(SPI_TypeDef* SPIx, uint8_t *rx_data_len, uint8_t *rx_data)
{
	typedef struct
	{
		uint8_t 				spi_header;
		uint8_t         cmd;
		uint8_t         signal;
		uint8_t					data_len;
		uint8_t 				data[BUFFER_SIZE_MAX];
		uint8_t 				xor;
	} nrf_to_stm32_cmd_type_t;

	nrf_to_stm32_cmd_type_t spi_revice_data;
	uint8_t retval[BUFFER_SIZE_MAX];
	uint8_t i = 0;
	uint8_t *temp_data = NULL;
	uint8_t revice_cal_xor = 0;

	*rx_data_len = 0;
	memset(spi_revice_data.data, 0xFF, BUFFER_SIZE_MAX);
	spi_revice_data.spi_header = 0x86;
	spi_cmd_type.data_len = 0x06;
	temp_data = (uint8_t *)&spi_cmd_type;

	/* 开始SPI传输 */
	NRF1_CSN_LOW();	
	memset(retval, 0, BUFFER_SIZE_MAX);
	//printf("SPI_RX:");

	for(i=0; i<spi_cmd_type.data_len; i++)
	{
		retval[i] = hal_nrf_rw(SPIx, *(temp_data+i));
		//printf(" %02x",retval[i]);
		if(i ==  3 && retval[0] == 0x86 && retval[3] < BUFFER_SIZE_MAX )
		{
			*rx_data_len = retval[3];
			spi_cmd_type.data_len += *rx_data_len;
		}
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF1_CSN_HIGH();	
	revice_cal_xor = XOR_Cal((uint8_t *)&(retval[1]), spi_cmd_type.data_len-3);
  //printf("%02x ?= %02x \r\n",retval[spi_cmd_type.data_len-2],revice_cal_xor);

  /* 若接收到数据校验正确 */
	if(retval[spi_cmd_type.data_len-2] == revice_cal_xor) 			
	{
		uint16_t uidpos = 0xFFFF;
		uint8_t Is_whitelist_uid;
		memcpy(rx_data, &retval[4],*rx_data_len);

		if(dtq_self_inspection_flg == 1)
		{
			uint8_t  Cmdtype;
			uint16_t AckTableLen;
			uint8_t *prdata;
			uint8_t *prssi;

			AckTableLen = retval[14+4];
			Cmdtype     = retval[14+4+AckTableLen+1];
			prdata      = retval+14+4+AckTableLen+2+1;
			prssi       = retval+2;

			if( Cmdtype == 0xF1 )
			{
				uint8_t test_buf[2] = {0,0};
				char str[20];

				nrf_transmit_parameter_t transmit_config;
				test_buf[0] = prdata[0];
				test_buf[1] = prssi[0];

				memcpy(transmit_config.dist,nrf_data.rbuf+5, 4 );
				transmit_config.package_type   = NRF_DATA_IS_USEFUL;
				transmit_config.transmit_count = SEND_DATA_COUNT;
				transmit_config.delay100us     = SEND_DATA_DELAY100US;
				transmit_config.is_pac_add     = PACKAGE_NUM_ADD;
				transmit_config.data_buf       = test_buf;
				transmit_config.data_len       = 2;
				transmit_config.sel_table      = SEND_DATA_ACK_TABLE+1;
				nrf_transmit_start( &transmit_config );
				b_print("{\r\n");
				b_print("  \"fun\": \"dtq_self_inspection\",\r\n");
				sprintf(str, "%010u" , *(uint32_t *)( nrf_data.rbuf+5));
				b_print("  \"card_id\": \"%s\",\r\n", str );
				memset(str,0,20);
				sprintf(str, "%d" , ( test_buf[1]));
				b_print("  \"dtq_rssi\": \"-%s\",\r\n", str );
				memset(str,0,20);
				sprintf(str, "%d" , ( test_buf[0]));
				b_print("  \"jsq_rssi\": \"-%s\"\r\n", str );
				b_print("}\r\n");
			}
		}

		Is_whitelist_uid = search_uid_in_white_list(rx_data+5,&uidpos);
		if( Is_whitelist_uid == OPERATION_SUCCESS )
			wl.uids[uidpos].rssi = retval[2];
		return 0;
	}
	else
	{
		return 1;
	}
}

void spi_write_tx_payload(const uint8_t *tx_pload, uint8_t length, uint8_t count, uint8_t delay100us)
{
	uint8_t retval[BUFFER_SIZE_MAX];
	uint16_t i = 0;
	uint8_t *pdata;

	/* 封装指令 */
	spi_cmd_type.spi_header = 0x86;
	spi_cmd_type.cmd        = 0x10;
	spi_cmd_type.count      = count;
	spi_cmd_type.delay100us = delay100us;
	spi_cmd_type.data_len   = length;
	memcpy(spi_cmd_type.data, tx_pload, length);
	spi_cmd_type.data[spi_cmd_type.data_len] = XOR_Cal((uint8_t *)&(spi_cmd_type.cmd), spi_cmd_type.data_len+4);
	spi_cmd_type.data[spi_cmd_type.data_len+1] = 0x76;
	
	/* 开始SPI传输 */
	NRF2_CSN_LOW();	
	memset(retval, 0, BUFFER_SIZE_MAX);
	//printf("SPI_TX:");
	pdata = (uint8_t *)&spi_cmd_type;
	for(i=0; i<spi_cmd_type.data_len+7; i++)
	{
#ifdef ZL_RP551_MAIN_E
	retval[i] = hal_nrf_rw(SPI1, *(pdata+i));
#endif

#ifdef ZL_RP551_MAIN_F
		retval[i] = hal_nrf_rw(SPI2, *(pdata+i));
#endif

#ifdef ZL_RP551_MAIN_H
		retval[i] = hal_nrf_rw(SPI2, *(pdata+i));
#endif
		//printf(" %02x",*(pdata+i));
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF2_CSN_HIGH();
}


void spi_set_cpu_rx_signal_ch( uint8_t rx_ch )
{
	uint16_t data = rx_ch;
	cpu_spi_cmd_typedef spi_cmd;
	uint8_t *pdata,i;
	
	/* 存储数据到FEE */
	EE_WriteVariable(CPU_RX_CH_POS_OF_FEE,data);

	/* 封装指令 */
	spi_cmd.header   = 0x86;
	spi_cmd.cmd      = 0x20;
	spi_cmd.channel  = rx_ch;
	spi_cmd.data_xor = XOR_Cal((uint8_t *)&(spi_cmd.cmd), 2);
	spi_cmd.end      = 0x76;

	/* 开始SPI传输 */
	NRF1_CSN_LOW();
	pdata = (uint8_t *)&spi_cmd;	

	//printf("SPI_RX_SET_CH:");

	for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
	{
		hal_nrf_rw(SPI1, *(pdata+i));
		//printf(" %02x",*(pdata+i));
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF1_CSN_HIGH();	
}

void spi_set_cpu_tx_signal_ch( uint8_t tx_ch )
{
	uint16_t data = tx_ch;
	cpu_spi_cmd_typedef spi_cmd;
	uint8_t i,retval[sizeof(cpu_spi_cmd_typedef)];
	
	/* 存储数据到FEE */
	EE_WriteVariable(CPU_TX_CH_POS_OF_FEE,data);
	
	/* 封装指令 */
	spi_cmd.header   = 0x86;
	spi_cmd.cmd      = 0x20;
	spi_cmd.channel  = tx_ch;
	spi_cmd.data_xor = XOR_Cal((uint8_t *)&(spi_cmd.cmd), 2);
	spi_cmd.end      = 0x76;
	
	/* 开始SPI传输 */
	NRF2_CSN_LOW();	
	//printf("SPI_TX_SET_CH:");
	memset(retval, 0, sizeof(cpu_spi_cmd_typedef));
	for(i=0; i<sizeof(cpu_spi_cmd_typedef); i++)
	{
#ifdef ZL_RP551_MAIN_E
		retval[i] = hal_nrf_rw(SPI1, *(pdata+i));
#endif

#if defined (ZL_RP551_MAIN_F) && defined (ZL_RP551_MAIN_H)
		retval[i] = hal_nrf_rw(SPI2, *(pdata+i));
#endif
		//printf(" %02x",*(pdata+i));
	}
	//printf("\r\n");

	/* 关闭SPI传输 */
	NRF2_CSN_HIGH();
}

/**************************************END OF FILE****************************/

