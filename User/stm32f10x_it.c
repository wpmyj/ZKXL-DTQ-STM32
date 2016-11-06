/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c
  * @author  MCD Application Team
  * @version V3.1.2
  * @date    09/28/2009
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "app_send_data_process.h"
/* uart global variables */

uint32_t clicker_test_printf_flg = 0;
timer_t clicker_time;
// revice part
Uart_MessageTypeDef uart_irq_revice_massage;
static uint32_t uart_rx_timeout = 0;
static uint32_t rf_tx_time_cnt = 0;
static uint32_t rf_tx_timeout_cnt = 0;

static bool     flag_uart_rxing = false;
static uint8_t  uart_status     = UartHEADER;

// send part
Uart_MessageTypeDef uart_irq_send_massage;
uint8_t uart_tx_status      = 0;

/* uart global variables */
extern nrf_communication_t	nrf_communication;


/* rf systick data */
volatile uint8_t rf_systick_status = 0; // 0 = IDLE
static uint32_t  rf_retransmit_timecnt = 0;
uint8_t spi_temp_buffer[4][256];
uint8_t write_index = 0, read_index = 0, Count = 0;
/******************************************************************************
  Function:rf_change_systick_status
  Description:
		修改systick的状态
  Input :
		rf_status: systick的新状态
  Output:
  Return:
  Others:None
******************************************************************************/
void rf_change_systick_status(uint8_t rf_status)
{
	rf_systick_status = rf_status;
	DebugLog("<%s> rf_systick_status = %d \r\n",__func__,rf_systick_status);
}

/******************************************************************************
  Function:rf_get_systick_status
  Description:
		获取systick的状态
  Input :
  Output:systick的新状态
  Return:
  Others:None
******************************************************************************/
uint8_t rf_get_systick_status(void)
{
	return rf_systick_status ;
}
/******************************************************************************
  Function:uart_clear_message
  Description:
		清除Message中的信息
  Input :
		Message: 协议Message的指针
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_clear_message( Uart_MessageTypeDef *Message )
{
	uint8_t i;
	uint8_t *pdata = (uint8_t*)(Message);

	for(i=0;i<PACKETSIZE;i++)
	{
		*pdata = 0;
		pdata++;
	}
}

/******************************************************************************
  Function:uart_revice_data_state_mechine
  Description:
		串口数据接收函数，提取有效数据存入缓存
  Input:None
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_revice_data_state_mechine( uint8_t data )
{
	static uint8_t	uart_rx_cnt     = 0;
	static uint8_t  temp_sign_len   = 0;

	switch(uart_status)
		{
			case UartHEADER:
				{
					if(UART_SOF == data)		//如果命令头为0x5C则开始接收，否则不接收
					{
						uart_irq_revice_massage.HEADER = data;
						uart_status =  UartTYPE;
						flag_uart_rxing = true;
					}
				}
				break;

			case UartTYPE:
				{
					uart_irq_revice_massage.TYPE = data;
					uart_status = UartSIGN;
					temp_sign_len = 0;
				}
				break;

			case UartSIGN:
				{
						uart_irq_revice_massage.SIGN[temp_sign_len++] = data;
						if( temp_sign_len == 4 )
						{
								uart_status = UartLEN;
						}
				}
				break;

			case UartLEN:
				{
					uart_irq_revice_massage.LEN = data;

					/*  若数据长度大于 236 */
					if(uart_irq_revice_massage.LEN > UART_NBUF)
					{
						uart_status =  UartHEADER;
						/* 清除 uart_irq_revice_massage 接收信息 */
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
					else if(uart_irq_revice_massage.LEN > 0)	//  DATA不为空
					{
						uart_status = UartDATA;
						uart_rx_cnt = 0;
					}
					else//  DATA为空
					{
						uart_status = UartXOR;
					}
				}
				break;

			case UartDATA:
				{
						uart_irq_revice_massage.DATA[uart_rx_cnt++] = data;
						/* 数据接收完成 */
						if(uart_rx_cnt == uart_irq_revice_massage.LEN)
								uart_status = UartXOR;
				}
				break;

			case UartXOR:
				{
						uart_irq_revice_massage.XOR = data;
						uart_status = UartEND;
				}
				break;

			case UartEND:
				{
					if(UART_EOF == data)
					{
						uint8_t UartMessageXor = XOR_Cal((uint8_t *)(&uart_irq_revice_massage.TYPE),
												uart_irq_revice_massage.LEN + 6 );
						uart_irq_revice_massage.END = data;

						if( uart_irq_revice_massage.XOR == UartMessageXor)
						{   /* 若校验通过，则接收数据OK可用 */
								if(BUFFERFULL != buffer_get_buffer_status(REVICE_RINGBUFFER))
								{
									serial_ringbuffer_write_data(REVICE_RINGBUFFER,&uart_irq_revice_massage);
								}
							  flag_uart_rxing = false;
								uart_status = UartHEADER;
								uart_clear_message(&uart_irq_revice_massage);
						}
						else
						{
							uart_clear_message(&uart_irq_revice_massage);
						}
					}
					else
					{
						uart_status = UartHEADER;
						uart_clear_message(&uart_irq_revice_massage);
						flag_uart_rxing = false;
					}
				}
				break;

			default:
				break;
		}
}

/******************************************************************************
  Function:uart_send_data_state_machine
  Description:
		串口数发送函数，从缓存中提取数据发送到上位机
  Input :
		status: uart tx status
  Output:
  Return:
  Others:None
******************************************************************************/
void uart_send_data_state_machine( void )
{
	static uint8_t uart_tx_cnt  = 0;
	static uint8_t *pdata;

	switch( uart_tx_status )
	{
		case 0:
			{
					if(BUFFEREMPTY == buffer_get_buffer_status(SEND_RINGBUFFER))
					{
						USART_ITConfig(USART1pos,USART_IT_TXE,DISABLE);
						return;
					}
					else
					{
						serial_ringbuffer_read_data(SEND_RINGBUFFER, &uart_irq_send_massage);
						pdata = (uint8_t *)(&uart_irq_send_massage);
						uart_tx_status = 1;
						uart_tx_cnt = *(pdata+6) + 7;
						uart_tx_status = 1;
					}
			}
			break;

		case 1:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					pdata = &(uart_irq_send_massage.XOR);
					uart_tx_cnt = 2;
					uart_tx_status = 2;
				}
			}
			break;

		case 2:
			{
				USART_SendData(USART1pos,*pdata);
				uart_tx_cnt--;
				pdata++;
				if( uart_tx_cnt == 0 )
				{
					uart_clear_message(&uart_irq_send_massage);
					uart_tx_status = 0;
				}
			}
			break;

		default:
			break;
	}
}

/** @addtogroup STM32F10x_StdPeriph_Examples
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/
#if defined(__CC_ARM) /*------------------RealView Compiler -----------------*/
__asm void GenerateSystemReset(void)
{
//	MOV R0, #1         	//;
//	MSR FAULTMASK, R0  	//; 清除FAULTMASK 禁止一切中断产生
//	LDR R0, =0xE000ED0C //;
//	LDR R1, =0x05Fa0004 //;
//	STR R1, [R0]       	//; 系统软件复位
//deadloop
//    B deadloop        //; 死循环使程序运行不到下面的代码
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

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
	while(1)
	{
		GenerateSystemReset();
	}
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
		GenerateSystemReset();
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	TimingDelay_Decrement();

	clicker_send_data_time_set1( SEND_DATA1_STATUS, SEND_DATA1_UPDATE_STATUS,SEND_DATA1_TIMEOUT);
	clicker_send_data_time_set1( SEND_DATA2_SEND_OVER_STATUS, SEND_DATA2_UPDATE_STATUS,SEND_DATA2_TIMEOUT);
	clicker_send_data_time_set1( SEND_DATA3_SEND_OVER_STATUS, SEND_DATA3_UPDATE_STATUS,SEND_DATA3_TIMEOUT);

	if(get_rf_retransmit_status() == 1)
	{
		rf_retransmit_timecnt++;
		if(rf_retransmit_timecnt == SEND_DATA4_TIMEOUT)
		{
			rf_retransmit_set_status(2);
			rf_retransmit_timecnt = 0;
		}
	}

	if(rf_systick_status == 3)
	{
		rf_tx_time_cnt++;

		/* 5S 产生心跳包 同时计数器清零 */
		if(rf_tx_time_cnt >= 5000)
		{
			rf_change_systick_status(4);
			rf_tx_time_cnt = 0;
		}
	}

	if(rf_systick_status == 1)
	{
		rf_tx_timeout_cnt++;
		/* 1S 结束在线状态统计，清零超时基数器 */
		if(rf_tx_timeout_cnt >= 1000)
		{
			rf_change_systick_status(2);
			rf_tx_timeout_cnt = 0;
		}
	}

	if(flag_uart_rxing)												//串口接收超时计数器
	{
		uart_rx_timeout++;
		if(uart_rx_timeout>5)										//5msc超时后重新开始接收
		{
			uart_clear_message(&uart_irq_revice_massage);
			flag_uart_rxing = false;
			uart_status = UartHEADER;
		}
	}

	if(timer_1ms++ > 1000)
	{
#ifdef ENABLE_WATCHDOG
		IWDG_ReloadCounter();													//定时喂狗
#endif //ENABLE_WATCHDOG
		timer_1ms = 0;
		time.second++;
		ledToggle(LGREEN);
		if(time.second >= 60)
		{
			time.second = 0;
			time.minute++;
			if(time.minute >= 60)
			{
				time.minute = 0;
				time.hour++;
				if(time.hour >= 24)
				{
					time.hour = 0;
				}
			}
		}
	}

	if(time_for_buzzer_on > 1)									//蜂鸣器开关延时时间
	{
		time_for_buzzer_on--;
	}

	if((time_for_buzzer_on == 0)&&(time_for_buzzer_off > 0))	//蜂鸣器响延时
	{
		time_for_buzzer_off--;
	}

	if(delay_nms)
	{
		delay_nms --;
	}


}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void USART1pos_IRQHandler(void)
{
	uint8_t uart_temp = 0;

	if(USART_GetITStatus(USART1pos, USART_IT_PE) != RESET)
	{
		// Enable SC_USART RXNE Interrupt (until receiving the corrupted byte)
		USART_ITConfig(USART1pos, USART_IT_RXNE, ENABLE);
		// Flush the SC_USART DR register
		USART_ReceiveData(USART1pos);
	}

	if(USART_GetITStatus(USART1pos, USART_IT_RXNE) != RESET)
	{
	  uart_temp = USART_ReceiveData(USART1pos);

		/* store it to uart_irq_revice_massage */
		uart_revice_data_state_mechine( uart_temp );

		uart_rx_timeout = 0;
	}


	if(USART_GetITStatus(USART1pos, USART_IT_TXE) != RESET)
  {
    uart_send_data_state_machine( );
	}
}

uint8_t irq_flag;
void RFIRQ_EXTI_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_LINE_RFIRQ) != RESET)
	{
		EXTI_ClearITPendingBit(EXTI_LINE_RFIRQ);

		/* 读取数据 */
		uesb_nrf_get_irq_flags(SPI1, &irq_flag, &nrf_communication.receive_len,
		                                         nrf_communication.receive_buf);
		/* 进行 crc 校验 */
		if( *(nrf_communication.receive_buf+1) == nrf_communication.jsq_uid[0] &&
			  *(nrf_communication.receive_buf+2) == nrf_communication.jsq_uid[1] &&
				*(nrf_communication.receive_buf+3) == nrf_communication.jsq_uid[2] &&
				*(nrf_communication.receive_buf+4) == nrf_communication.jsq_uid[3])
		{
			uint8_t send_data_status = get_clicker_send_data_status();
			memcpy(spi_temp_buffer[write_index],nrf_communication.receive_buf,nrf_communication.receive_buf[14]+17);
			spi_temp_buffer[write_index][nrf_communication.receive_buf[14]+17] = send_data_status;
			write_index = (write_index + 1) % 4;
			Count++;
		}
	}
	ledToggle(LBLUE);
}

/**
  * @}
  */

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
